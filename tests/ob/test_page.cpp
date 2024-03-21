#include <ob/orderbook.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>

struct buy_t {
    static constexpr auto value = ob::md_side::buy;
};

struct sell_t {
    static constexpr auto value = ob::md_side::sell;
};

TEMPLATE_TEST_CASE("page", "[page]", buy_t, sell_t) {
    constexpr auto side = TestType::value;
    constexpr auto px = 100;
    auto dur = ob::timestamp_t::duration{1};
    auto page = ob::page<side>{};
    REQUIRE(std::empty(page));
    REQUIRE(std::size(page) == 0);
    REQUIRE(page.find(px) == nullptr);

    auto order_0 = ob::md_order{.px = px, .qty = 42};
    auto order_ptr_0 = std::addressof(order_0);

    auto order_1 = ob::md_order{.px = px, .qty = 42};
    auto order_ptr_1 = std::addressof(order_1);

    SECTION("adding order changes size") {
        page.add_order(order_ptr_0);
        REQUIRE(std::size(page) == 1);
        REQUIRE(not std::empty(page));
        auto maybe_level = page.find(px);
        REQUIRE(maybe_level != nullptr);
        REQUIRE(maybe_level->px == px);
        REQUIRE(maybe_level->qty == 42);
        REQUIRE(maybe_level->oc == 1);
    }
    SECTION("deleting last order changes size") {
        page.add_order(order_ptr_0);
        page.delete_order(order_ptr_0);
        REQUIRE(std::empty(page));
        REQUIRE(std::size(page) == 0);
        REQUIRE(page.find(px) == nullptr);
    }
    SECTION("deleting from level with two orders doesn't change size") {
        page.add_order(order_ptr_0);
        page.add_order(order_ptr_1);
        page.delete_order(order_ptr_0);
        REQUIRE(not std::empty(page));
        REQUIRE(std::size(page) == 1);
    }
    SECTION("reducing order quantity with quantity remaining doesn't change size") {
        page.add_order(order_ptr_0);
        page.reduce(order_ptr_0, 10);
        REQUIRE(not std::empty(page));
        REQUIRE(std::size(page) == 1);
    }
    SECTION("reducing order quantity to zero changes size") {
        page.add_order(order_ptr_0);
        page.reduce(order_ptr_0, 42);
        REQUIRE(std::empty(page));
        REQUIRE(std::size(page) == 0);
    }
}