#include <ob/orderbook.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>

TEST_CASE("add orders", "[level]") {
    auto order_0 = ob::md_order{.px = 100, .qty = 42};
    auto order_ptr_0 = std::addressof(order_0);

    auto level = ob::md_price_level{};
    SECTION("inject first order at construction") {
        level = ob::md_price_level{.px = 100,
                                   .qty = 42,
                                   .oc = 1,
                                   .head_order = order_ptr_0,
                                   .tail_order = order_ptr_0};
    }
    SECTION("construct empty") {
        level = ob::md_price_level{.px = 100,
                                   .qty = 0,
                                   .oc = 0,
                                   .head_order = nullptr,
                                   .tail_order = nullptr};
        REQUIRE(level.oc == 0);
        level.push_back(order_ptr_0);
        REQUIRE(level.oc == 1);
        REQUIRE(level.qty == 42);
    }

    auto order_1 = ob::md_order{.px = 100, .qty = 23};
    auto order_ptr_1 = std::addressof(order_1);

    level.push_back(order_ptr_1);
    REQUIRE(level.oc == 2);
    REQUIRE(level.qty == 42 + 23);

    REQUIRE(order_0.next_order == order_ptr_1);
    REQUIRE(order_1.next_order == nullptr);

    REQUIRE(order_0.prev_order == nullptr);
    REQUIRE(order_1.prev_order == order_ptr_0);

    auto order_2 = ob::md_order{.px = 100, .qty = 12};
    auto order_ptr_2 = std::addressof(order_2);

    level.push_back(order_ptr_2);
    REQUIRE(level.oc == 3);
    REQUIRE(level.qty == 42 + 23 + 12);

    REQUIRE(order_0.next_order == order_ptr_1);
    REQUIRE(order_1.next_order == order_ptr_2);
    REQUIRE(order_2.next_order == nullptr);

    REQUIRE(order_0.prev_order == nullptr);
    REQUIRE(order_1.prev_order == order_ptr_0);
    REQUIRE(order_2.prev_order == order_ptr_1);
}

TEST_CASE("remove orders", "[level]") {
    auto order_0 = ob::md_order{.px = 100, .qty = 42};
    auto order_ptr_0 = std::addressof(order_0);
    auto level = ob::md_price_level{.px = 100,
                                    .qty = 42,
                                    .oc = 1,
                                    .head_order = order_ptr_0,
                                    .tail_order = order_ptr_0};

    auto order_1 = ob::md_order{.px = 100, .qty = 23};
    auto order_ptr_1 = std::addressof(order_1);
    level.push_back(order_ptr_1);

    auto order_2 = ob::md_order{.px = 100, .qty = 12};
    auto order_ptr_2 = std::addressof(order_2);
    level.push_back(order_ptr_2);

    SECTION("remove first order") {
        level.erase(order_ptr_0);
        REQUIRE(level.oc == 2);
        REQUIRE(level.qty == 23 + 12);
        REQUIRE(level.head_order == order_ptr_1);
        REQUIRE(level.tail_order == order_ptr_2);
        REQUIRE(order_1.prev_order == nullptr);
        REQUIRE(order_2.prev_order == order_ptr_1);
        REQUIRE(order_0.next_order == nullptr);
        REQUIRE(order_0.prev_order == nullptr);
    }
    SECTION("remove middle order") {
        level.erase(order_ptr_1);
        REQUIRE(level.oc == 2);
        REQUIRE(level.qty == 42 + 12);
        REQUIRE(level.head_order == order_ptr_0);
        REQUIRE(level.tail_order == order_ptr_2);
        REQUIRE(order_0.prev_order == nullptr);
        REQUIRE(order_2.prev_order == order_ptr_0);
        REQUIRE(order_1.next_order == nullptr);
        REQUIRE(order_1.prev_order == nullptr);
    }
    SECTION("remove last order") {
        level.erase(order_ptr_2);
        REQUIRE(level.oc == 2);
        REQUIRE(level.qty == 42 + 23);
        REQUIRE(level.head_order == order_ptr_0);
        REQUIRE(level.tail_order == order_ptr_1);
        REQUIRE(order_0.prev_order == nullptr);
        REQUIRE(order_1.next_order == nullptr);
        REQUIRE(order_2.next_order == nullptr);
        REQUIRE(order_2.prev_order == nullptr);
    }
    SECTION("remove all orders in fifo order") {
        level.erase(order_ptr_0);
        level.erase(order_ptr_1);
        level.erase(order_ptr_2);
        REQUIRE(level.oc == 0);
        REQUIRE(level.qty == 0);
        REQUIRE(level.head_order == nullptr);
        REQUIRE(level.tail_order == nullptr);
    }
    SECTION("remove all orders in lifo order") {
        level.erase(order_ptr_2);
        level.erase(order_ptr_1);
        level.erase(order_ptr_0);
        REQUIRE(level.oc == 0);
        REQUIRE(level.qty == 0);
        REQUIRE(level.head_order == nullptr);
        REQUIRE(level.tail_order == nullptr);
    }
}

TEST_CASE("reduce order quantity", "[level]") {
    auto order_0 = ob::md_order{.px = 100, .qty = 42};
    auto order_ptr_0 = std::addressof(order_0);
    auto level = ob::md_price_level{.px = 100,
                                    .qty = 42,
                                    .oc = 1,
                                    .head_order = order_ptr_0,
                                    .tail_order = order_ptr_0};

    SECTION("cancel all") {
        level.reduce(order_ptr_0, 42);
        REQUIRE(level.oc == 0);
        REQUIRE(level.qty == 0);
        REQUIRE(level.head_order == nullptr);
        REQUIRE(level.tail_order == nullptr);
    }
    SECTION("cancel some") {
        level.reduce(order_ptr_0, 23);
        REQUIRE(level.oc == 1);
        REQUIRE(level.qty == 42 - 23);
        REQUIRE(level.head_order == order_ptr_0);
        REQUIRE(level.tail_order == order_ptr_0);
    }
}

TEST_CASE("iterate", "[level]") {
    auto order_0 = ob::md_order{.px = 100, .qty = 42};
    auto order_ptr_0 = std::addressof(order_0);
    auto level = ob::md_price_level{.px = 100,
                                    .qty = 42,
                                    .oc = 1,
                                    .head_order = order_ptr_0,
                                    .tail_order = order_ptr_0};

    auto order_1 = ob::md_order{.px = 100, .qty = 23};
    auto order_ptr_1 = std::addressof(order_1);
    level.push_back(order_ptr_1);

    auto order_2 = ob::md_order{.px = 100, .qty = 12};
    auto order_ptr_2 = std::addressof(order_2);
    level.push_back(order_ptr_2);

    SECTION("forward") {
        auto it = std::begin(level);
        auto end = std::end(level);
        REQUIRE(it != end);
        REQUIRE(std::addressof(*it) == order_ptr_0);
        REQUIRE(++it != end);
        REQUIRE(std::addressof(*it) == order_ptr_1);
        it++;
        REQUIRE(it != end);
        REQUIRE(std::addressof(*it) == order_ptr_2);
        REQUIRE(++it == end);
    }
    SECTION("after deleting first order") {
        level.erase(order_ptr_0);
        auto it = std::begin(level);
        auto end = std::end(level);
        REQUIRE(it != end);
        REQUIRE(std::addressof(*it) == order_ptr_1);
        REQUIRE(++it != end);
        REQUIRE(std::addressof(*it) == order_ptr_2);
        REQUIRE(++it == end);
    }
    SECTION("after deleting last order") {
        level.erase(order_ptr_2);
        auto it = std::begin(level);
        auto end = std::end(level);
        REQUIRE(it != end);
        REQUIRE(std::addressof(*it) == order_ptr_0);
        REQUIRE(++it != end);
        REQUIRE(std::addressof(*it) == order_ptr_1);
        REQUIRE(++it == end);
    }
    SECTION("after deleting middle order") {
        level.erase(order_ptr_1);
        auto it = std::begin(level);
        auto end = std::end(level);
        REQUIRE(it != end);
        REQUIRE(std::addressof(*it) == order_ptr_0);
        REQUIRE(++it != end);
        REQUIRE(std::addressof(*it) == order_ptr_2);
        REQUIRE(++it == end);
    }
    SECTION("after deleting all orders in fifo order") {
        level.erase(order_ptr_0);
        level.erase(order_ptr_1);
        level.erase(order_ptr_2);
        REQUIRE(std::begin(level) == std::end(level));
        REQUIRE(std::cbegin(level) == std::cend(level));
    }
    SECTION("after deleting all orders in lifo order") {
        level.erase(order_ptr_2);
        level.erase(order_ptr_1);
        level.erase(order_ptr_0);
        REQUIRE(std::begin(level) == std::end(level));
        REQUIRE(std::cbegin(level) == std::cend(level));
    }
}