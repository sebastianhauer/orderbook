#include <sh/mem/storage.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>

TEST_CASE("allocate", "[fixed_storage]") {
    auto storage = sh::mem::fixed_storage<16>{};
    REQUIRE(storage.remaining() == 16);
    REQUIRE(storage.used() == 0);

    SECTION("allocating zero bytes is allowed") {
        auto ptr = storage.allocate(0, 1);
        REQUIRE(ptr != nullptr);
        REQUIRE(storage.remaining() == 16);
        REQUIRE(storage.used() == 0);
    }
    SECTION("allocations remain aligned") {
        {
            auto ptr = storage.allocate(1, 1);
            REQUIRE(reinterpret_cast<std::uintptr_t>(ptr) % 1 == 0);
            REQUIRE(ptr == storage.data());
            REQUIRE(storage.remaining() == 15);
            REQUIRE(storage.used() == 1);
        }
        {
            auto ptr = storage.allocate(2, 2);
            REQUIRE(reinterpret_cast<std::uintptr_t>(ptr) % 2 == 0);
            REQUIRE(ptr == storage.data() + 2);
            REQUIRE(storage.remaining() == 12);
            REQUIRE(storage.used() == 4);
        }
        {
            auto ptr = storage.allocate(8, 8);
            REQUIRE(reinterpret_cast<std::uintptr_t>(ptr) % 8 == 0);
            REQUIRE(ptr == storage.data() + 2 + 2 + 4);
            REQUIRE(storage.remaining() == 0);
            REQUIRE(storage.used() == 16);
        }
        {
            REQUIRE_THROWS_AS(storage.allocate(1, 1), std::bad_alloc);
        }
    }
}