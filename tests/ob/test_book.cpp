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

TEMPLATE_TEST_CASE("book", "[book]", buy_t, sell_t) {
    constexpr auto side = TestType::value;
    auto dur = ob::timestamp_t::duration{1};
    auto book = ob::book{};
    REQUIRE(book.empty());
    REQUIRE(book.size() == 0);
    REQUIRE(book.page<side>().size() == 0);

    SECTION("add order") {
        REQUIRE(book.empty());
        book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
        REQUIRE(not book.empty());
        REQUIRE(book.size() == 1);
        REQUIRE(book.page<side>().size() == 1);
    }
    SECTION("add order and find") {
        REQUIRE(book.empty());
        auto maybe_order = book.find_order(1);
        REQUIRE(maybe_order == nullptr);
        book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
        maybe_order = book.find_order(1);
        REQUIRE(maybe_order != nullptr);
    }
    SECTION("delete order") {
        REQUIRE(book.empty());
        book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
        book.delete_order(1);
        REQUIRE(book.empty());
        REQUIRE(book.size() == 0);
        REQUIRE(book.page<side>().size() == 0);
    }
    SECTION("reduce order quantity") {
        REQUIRE(book.empty());
        book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
        book.reduce_order_quantity(1, 10);
        REQUIRE(not book.empty());
        REQUIRE(book.size() == 1);
        REQUIRE(book.page<side>().size() == 1);
    }
    SECTION("reduce order quantity to zero") {
        REQUIRE(book.empty());
        book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
        book.reduce_order_quantity(1, 42);
        REQUIRE(book.empty());
        REQUIRE(book.size() == 0);
        REQUIRE(book.page<side>().size() == 0);
    }
    SECTION("replace order") {
        REQUIRE(book.empty());
        book.add_order(ob::timestamp_t{}, 1, 100, 42, side);
        book.replace_order(ob::timestamp_t{}, 1, 2, 100, 42);
        REQUIRE(not book.empty());
        REQUIRE(book.size() == 1);
        REQUIRE(book.page<side>().size() == 1);
    }
}