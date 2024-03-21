#pragma once

#include <gtl/phmap.hpp>

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace ob {
using order_id_t = std::uint64_t;
using price_t = std::int64_t;
using quantity_t = std::int32_t;
using order_count_t = std::int32_t;
using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;

static_assert(sizeof(timestamp_t) == 8, "timestamp_t must be 8 bytes");

enum class md_side : std::uint16_t {
    buy = 0,
    sell = 1,
};

struct md_order {
    timestamp_t ts;           // 8
    order_id_t id;            // 8
    price_t px;               // 8
    quantity_t qty;           // 4
    md_side side;             // 2
    md_order* prev_order{};   // 8
    md_order* next_order{};   // 8
};

static_assert(sizeof(md_order) == 48, "md_order must be 48 bytes");

struct md_price_level {
    price_t px;               // 8
    quantity_t qty;           // 4
    order_count_t oc;         // 4
    md_order* head_order{};   // 8
    md_order* tail_order{};   // 8

    bool empty() const noexcept { return oc == 0; }

    void push_back(md_order* const order) {
        assert(order->px == px);
        if (tail_order) {
            tail_order->next_order = order;
            order->prev_order = tail_order;
            tail_order = order;
        } else {
            head_order = tail_order = order;
        }
        ++oc;
        qty += order->qty;
    }

    void erase(md_order* const order) {
        assert(order->px == px);
        if (head_order == order) {
            head_order = order->next_order;
        }
        if (tail_order == order) {
            tail_order = order->prev_order;
        }
        if (order->prev_order) {
            order->prev_order->next_order = order->next_order;
        }
        if (order->next_order) {
            order->next_order->prev_order = order->prev_order;
        }
        order->prev_order = order->next_order = nullptr;
        assert(oc > 0);
        --oc;
        assert(qty >= order->qty);
        qty -= order->qty;
    }

    void reduce(md_order* const order, const quantity_t reduce_qty) {
        assert(order->px == px);
        assert(order->qty >= reduce_qty);
        assert(qty >= reduce_qty);
        order->qty -= reduce_qty;
        qty -= reduce_qty;
        if (order->qty == 0) {
            erase(order);
        }
    }

    struct const_iterator {
        using value_type = md_order;
        using difference_type = std::ptrdiff_t;
        using pointer = const md_order*;
        using reference = const md_order&;
        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator() = default;
        const_iterator(const md_order* const order) : order_{order} {}

        reference operator*() const noexcept { return *order_; }
        pointer operator->() const noexcept { return order_; }

        const_iterator& operator++() noexcept {
            order_ = order_->next_order;
            return *this;
        }

        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            order_ = order_->next_order;
            return tmp;
        }

        const_iterator& operator--() noexcept {
            order_ = order_->prev_order;
            return *this;
        }

        const_iterator operator--(int) noexcept {
            auto tmp = *this;
            order_ = order_->prev_order;
            return tmp;
        }

        bool operator==(const const_iterator& other) const noexcept {
            return order_ == other.order_;
        }

        bool operator!=(const const_iterator& other) const noexcept {
            return order_ != other.order_;
        }

      private:
        const md_order* order_{};
    };

    const_iterator cbegin() const noexcept {
        return const_iterator{head_order};
    }
    const_iterator cend() const noexcept { return const_iterator{}; }
    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator end() const noexcept { return cend(); }
};
static_assert(sizeof(md_price_level) == 32, "md_price_level must be 32 bytes");

template <md_side side>
struct page {
    void add_order(md_order* const order) {
        if (auto [it, inserted] = price_to_level_.try_emplace(
                order->px, md_price_level{.px = order->px,
                                          .qty = order->qty,
                                          .oc = 1,
                                          .head_order = order,
                                          .tail_order = order});
            not inserted) {
            auto& level = it->second;
            level.push_back(order);
        }
    }

    void delete_order(md_order* const order) {
        if (auto it = price_to_level_.find(order->px);
            it != std::end(price_to_level_)) {
            auto& level = it->second;
            level.erase(order);
            if (std::empty(level)) {
                price_to_level_.erase(it);
            }
        }
    }

    void reduce(md_order* const order, const quantity_t reduce_qty) {
        if (auto it = price_to_level_.find(order->px);
            it != std::end(price_to_level_)) {
            auto& level = it->second;
            level.reduce(order, reduce_qty);
            if (std::empty(level)) {
                price_to_level_.erase(it);
            }
        }
    }

    std::size_t size() const { return std::size(price_to_level_); }
    bool empty() const { return std::empty(price_to_level_); }

  private:
    gtl::flat_hash_map<price_t, md_price_level> price_to_level_{};
};

struct book {
    using bids_t = page<md_side::buy>;
    using asks_t = page<md_side::sell>;

    void add_order(const timestamp_t ts, const order_id_t id, const price_t px,
                   const quantity_t qty, const md_side side) {
        auto order = new md_order{ts, id, px, qty, side};
        if (auto [it, inserted] = id_to_order_.emplace(id, order); inserted) {
            if (side == md_side::buy) {
                bids_.add_order(order);
            } else {
                asks_.add_order(order);
            }
        } else {
            delete order;
            // TODO: error
        }
    }

    void delete_order(const order_id_t id) {
        if (auto it = id_to_order_.find(id); it != std::end(id_to_order_)) {
            delete_order_iter(it);
        }
    }

    void reduce_order_quantity(const order_id_t id,
                               const quantity_t reduce_qty) {
        if (auto it = id_to_order_.find(id); it != std::end(id_to_order_)) {
            auto order = it->second;
            if (order->qty > reduce_qty) {
                order->qty -= reduce_qty;
            } else if (order->qty == reduce_qty) {
                delete_order_iter(it);
            } else {
                assert(false);
                return;
            }
            if (order->side == md_side::buy) {
                bids_.reduce(order, reduce_qty);
            } else {
                asks_.reduce(order, reduce_qty);
            }
        }
    }

    void replace_order(const timestamp_t ts, const order_id_t orig_id,
                       const order_id_t new_id, const price_t px,
                       const quantity_t qty) {
        if (auto it = id_to_order_.find(orig_id);
            it != std::end(id_to_order_)) {
            const auto side = it->second->side;
            delete_order_iter(it);
            add_order(ts, new_id, px, qty, side);
        }
    }

    template <md_side side>
    const page<side>& page() const noexcept {
        if constexpr (side == md_side::buy) {
            return bids_;
        } else {
            return asks_;
        }
    }

    // const bids_t &bids() const noexcept { return bids_; }
    // const asks_t &asks() const noexcept { return asks_; }

    std::optional<const md_order*>
    find_order(const order_id_t id) const noexcept {
        if (auto it = id_to_order_.find(id); it != std::cend(id_to_order_)) {
            return it->second;
        }
        return std::nullopt;
    }

    bool empty() const noexcept { return std::empty(id_to_order_); }
    std::size_t size() const noexcept { return std::size(id_to_order_); }

  private:
    void delete_order_iter(auto id_to_order_it) {
        auto order = id_to_order_it->second;
        if (order->side == md_side::buy) {
            bids_.delete_order(order);
        } else {
            asks_.delete_order(order);
        }
        delete order;
        id_to_order_.erase(id_to_order_it);
    }

    gtl::flat_hash_map<order_id_t, md_order*> id_to_order_{};
    bids_t bids_{};
    asks_t asks_{};
};

}   // namespace ob