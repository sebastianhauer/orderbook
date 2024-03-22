#pragma once

#include "sh/mem/storage.hpp"

#include <memory>

namespace sh::mem {

struct storage_allocator_tag {};

template <typename T, typename StorageT, typename Tag = storage_allocator_tag>
struct storage_allocator {
  private:
    using storage_type = StorageT;

  public:
    using value_type = T;

    template <typename U>
    struct rebind {
        using other = storage_allocator<U, StorageT, Tag>;
    };

    explicit storage_allocator(storage_type* const storage) noexcept
        : storage_{storage} {}

    template <typename U>
    constexpr storage_allocator(const storage_allocator<U, StorageT, Tag>& other) noexcept
        : storage_{other.storage_} {}

    T* allocate(const std::size_t n) {
        return reinterpret_cast<T*>(
            storage_->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T* const p, const std::size_t n) {
        storage_->deallocate(p, n * sizeof(T), alignof(T));
    }

  private:
    template<typename U, typename StorageU, typename TagU>
    friend struct storage_allocator;

    storage_type* storage_;
};

template <class T, class U, typename StorageT, typename Tag>
constexpr bool
operator==(const storage_allocator<T, StorageT, Tag>&, const storage_allocator<U, StorageT, Tag>&) noexcept {
    return true;
}

template <class T, class U, typename StorageT, typename Tag>
constexpr bool
operator!=(const storage_allocator<T, StorageT, Tag>&, const storage_allocator<U, StorageT, Tag>&) noexcept {
    return false;
}

}   // namespace sh::mem