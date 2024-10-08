#pragma once

#include <boost/core/demangle.hpp>

#include <array>
#include <iostream>
#include <memory>

namespace sh::mem {

namespace {

template <typename T>
struct demange {
    using type = T;
};

template <typename T>
std::ostream&
operator<<(std::ostream& os, const demange<T>&) {
    return os << boost::core::demangle(typeid(T).name());
}

}   // namespace

struct tracing_allocator_tag {};

template <typename T, typename Tag = tracing_allocator_tag>
struct tracing_allocator {
    using value_type = T;

    template <typename U>
    struct rebind {
        using other = tracing_allocator<U, Tag>;
    };

    constexpr tracing_allocator() noexcept = default;

    template <typename U>
    constexpr tracing_allocator(const tracing_allocator<U, Tag>&) noexcept {}

    T* allocate(const std::size_t n) {
        std::cout << demange<Tag>{} << ": allocate " << n << " * sizeof "
                  << sizeof(T) << " = " << n * sizeof(T) << " bytes of "
                  << demange<T>{} << std::endl;
        return std::allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const std::size_t n) {
        std::cout << demange<Tag>{} << ": deallocate " << n << " * sizeof "
                  << sizeof(T) << " = " << n * sizeof(T) << " bytes of "
                  << demange<T>{} << std::endl;
        std::allocator<T>{}.deallocate(p, n);
    }
};

template <typename T, typename U, typename Tag>
constexpr bool
operator==(const tracing_allocator<T, Tag>&,
           const tracing_allocator<U, Tag>&) noexcept {
    return true;
}

template <typename T, typename U, typename Tag>
constexpr bool
operator!=(const tracing_allocator<T, Tag>&,
           const tracing_allocator<U, Tag>&) noexcept {
    return false;
}

}   // namespace sh::mem