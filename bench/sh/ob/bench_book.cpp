#include <sh/ob/orderbook.hpp>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>
#include <iostream>

TEST_CASE("book", "[book]") {
    auto book = sh::ob::book{};

    auto rd = std::random_device{};   // obtain a random number from hardware
    auto gen = std::mt19937{rd()};    // seed the generator
    auto bid_px_dist = std::uniform_int_distribution<>(100, 300);
    auto ask_px_dist = std::uniform_int_distribution<>(301, 501);
    auto qty_dist = std::uniform_int_distribution<>(1, 100);

    constexpr auto N = 100'000u;

    auto bid_order_ids = std::vector<sh::ob::order_id_t>{N};
    std::iota(std::begin(bid_order_ids), std::end(bid_order_ids), 1);
    std::shuffle(std::begin(bid_order_ids), std::end(bid_order_ids), gen);

    auto ask_order_ids = std::vector<sh::ob::order_id_t>{N};
    std::iota(std::begin(ask_order_ids), std::end(ask_order_ids), N + 1);
    std::shuffle(std::begin(ask_order_ids), std::end(ask_order_ids), gen);

    auto bids = std::vector<sh::ob::md_order>{};
    auto asks = std::vector<sh::ob::md_order>{};
    bids.reserve(N);
    asks.reserve(N);
    for (auto i = 0u; i < N; ++i) {
        bids.push_back(sh::ob::md_order{sh::ob::timestamp_t{}, bid_order_ids[i],
                                    bid_px_dist(gen), qty_dist(gen),
                                    sh::ob::md_side::buy});
        asks.push_back(sh::ob::md_order{sh::ob::timestamp_t{}, ask_order_ids[i],
                                    ask_px_dist(gen), qty_dist(gen),
                                    sh::ob::md_side::sell});
    }
    BENCHMARK("add orders") {
        for (auto i = 0u; i < N; ++i) {
            const auto& bid_order = bids[i];
            const auto& ask_order = asks[i];
            book.add_order(bid_order.ts, bid_order.id, bid_order.px,
                           bid_order.qty, bid_order.side);
            book.add_order(ask_order.ts, ask_order.id, ask_order.px,
                           ask_order.qty, ask_order.side);
        }
        return std::size(book);
    };

    auto bid_order_ids_to_delete = bid_order_ids;
    auto ask_order_ids_to_delete = ask_order_ids;
    std::shuffle(std::begin(bid_order_ids_to_delete),
                 std::end(bid_order_ids_to_delete), gen);
    std::shuffle(std::begin(ask_order_ids_to_delete),
                 std::end(ask_order_ids_to_delete), gen);

    BENCHMARK_ADVANCED("delete orders")(Catch::Benchmark::Chronometer meter) {
        for (auto i = 0u; i < N; ++i) {
            const auto& bid_order = bids[i];
            const auto& ask_order = asks[i];
            book.add_order(bid_order.ts, bid_order.id, bid_order.px,
                           bid_order.qty, bid_order.side);
            book.add_order(ask_order.ts, ask_order.id, ask_order.px,
                           ask_order.qty, ask_order.side);
        }
        meter.measure([&](int) {
            for (auto i = 0u; i < N; ++i) {
                book.delete_order(bid_order_ids_to_delete[i]);
                book.delete_order(ask_order_ids_to_delete[i]);
            }
            return std::size(book);
        });
    };
}