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

    auto order_0 = ob::md_order{.px = px, .qty = 42};
    auto order_ptr_0 = std::addressof(order_0);
    SECTION("adding order changes size") {
        page.add_order(order_ptr_0);
        REQUIRE(std::size(page) == 1);
        REQUIRE(not std::empty(page));
    }
    // SECTION("add order") {
    //     REQUIRE(book.empty());
    //     book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
    //     REQUIRE(not book.empty());
    //     REQUIRE(book.size() == 1);
    //     REQUIRE(book.page<side>().size() == 1);
    // }
    // SECTION("add order and find") {
    //     REQUIRE(book.empty());
    //     auto maybe_order = book.find_order(1);
    //     REQUIRE(not maybe_order.has_value());
    //     book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
    //     maybe_order = book.find_order(1);
    //     REQUIRE(maybe_order.has_value());
    // }
    // SECTION("delete order") {
    //     REQUIRE(book.empty());
    //     book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
    //     book.delete_order(1);
    //     REQUIRE(book.empty());
    //     REQUIRE(book.size() == 0);
    //     REQUIRE(book.page<side>().size() == 0);
    // }
    // SECTION("reduce order quantity") {
    //     REQUIRE(book.empty());
    //     book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
    //     book.reduce_order_quantity(1, 10);
    //     REQUIRE(not book.empty());
    //     REQUIRE(book.size() == 1);
    //     REQUIRE(book.page<side>().size() == 1);
    // }
    // SECTION("reduce order quantity to zero") {
    //     REQUIRE(book.empty());
    //     book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
    //     book.reduce_order_quantity(1, 42);
    //     REQUIRE(book.empty());
    //     REQUIRE(book.size() == 0);
    //     REQUIRE(book.page<side>().size() == 0);
    // }
    // SECTION("replace order") {
    //     REQUIRE(book.empty());
    //     book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
    //     book.replace_order(ob::timestamp_t{}, 1, 2, 100, 42);
    //     REQUIRE(not book.empty());
    //     REQUIRE(book.size() == 1);
    //     REQUIRE(book.page<side>().size() == 1);
    // }
}