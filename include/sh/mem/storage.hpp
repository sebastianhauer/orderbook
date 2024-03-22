#pragma once

#include <array>
#include <cstddef>
#include <new>

namespace sh::mem {

constexpr std::size_t cache_line_alignment = 64;

template <std::size_t N, std::size_t Alignment = cache_line_alignment>
struct fixed_storage {

    std::size_t remaining() const noexcept { return N - pos_; }

    void* allocate(std::size_t n, std::size_t align) {
        if (n > remaining()) {
            throw std::bad_array_new_length{};
        }
        if (align > Alignment) {
            return std::bad_alloc{};
        }
        auto ptr = &data_[pos_];
        pos_ += size;
        return ptr;
    }

  private:
    std::size_t pos_{};
    alignas(Alignment) std::array<std::byte, N> data_{};
};

}   // namespace sh::mem