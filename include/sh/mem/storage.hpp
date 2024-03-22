#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <new>

namespace sh::mem {

#ifdef __cpp_lib_hardware_interference_size
constexpr std::size_t cache_line_alignment = std::hardware_destructive_interference_size;
#else
constexpr std::size_t cache_line_alignment = 64;
#endif

template <std::size_t N, std::size_t Alignment = cache_line_alignment>
struct fixed_storage {
    fixed_storage() noexcept : data_ptr_{std::data(data_)} {}

    constexpr const std::byte* data() const noexcept {
        return std::data(data_);
    }

    constexpr std::size_t max_size() const noexcept { return N; }
    std::size_t remaining() const noexcept { return rem_size_; }
    std::size_t used() const noexcept { return N - rem_size_; }

    void* allocate(const std::size_t n, const std::size_t align) {
        if (void* ptr = data_ptr_; std::align(align, n, ptr, rem_size_)) {
            data_ptr_ = static_cast<std::byte*>(ptr) + n;
            rem_size_ -= n;
            return ptr;
        }
        throw std::bad_alloc{};
    }

    void deallocate(void* const ptr, const std::size_t n,
                    const std::size_t align) {}

  private:
    std::size_t rem_size_{N};
    std::byte* data_ptr_{};
    alignas(Alignment) std::array<std::byte, N> data_{};
};

}   // namespace sh::mem