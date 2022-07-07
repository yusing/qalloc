// Copyright 2022 yusing. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// @file qalloc/internal/allocator_impl.hpp
/// @brief qalloc allocator implementation header file.
/// @author yusing
/// @date 2022-07-02

#ifndef QALLOC_ALLOCATOR_IMPL_HPP
#define QALLOC_ALLOCATOR_IMPL_HPP

#include <qalloc/internal/allocator.hpp>
#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/pool.hpp>
#include <qalloc/internal/global_pool.hpp>
#include <qalloc/internal/pointer.hpp>

QALLOC_BEGIN

template <typename T, bool detailed> allocator_base<T, detailed>::
allocator_base() noexcept
    : m_pool_ptr(internal::get_pool<T>()) {}

template <typename T, bool detailed> allocator_base<T, detailed>::
allocator_base(pool_pointer p_pool) noexcept
    : m_pool_ptr(p_pool) {}

template <typename T, bool detailed> allocator_base<T, detailed>::
allocator_base(const allocator_base& other) noexcept
    : m_pool_ptr(other.m_pool_ptr) {}

template <typename T, bool detailed> template <typename U, bool U_detailed> allocator_base<T, detailed>::
allocator_base(const allocator_base<U, U_detailed>& other) noexcept
    : m_pool_ptr(other.pool()) {}

template <typename T, bool detailed> allocator_base<T, detailed> & allocator_base<T, detailed>::
operator=(const allocator_base<T, detailed>& other) noexcept {
    if (this != &other) {
        m_pool_ptr = other.m_pool_ptr;
    }
    return *this;
}

template <typename T, bool detailed> typename allocator_base<T, detailed>::pointer allocator_base<T, detailed>::
allocate(size_type n_elements) {
    QALLOC_ASSERT(n_elements > 0);
    QALLOC_IF_CONSTEXPR(detailed) {
        return reinterpret_cast<pointer>(m_pool_ptr->template detailed_allocate<T>(n_elements * sizeof(T)));
    }
    return reinterpret_cast<pointer>(m_pool_ptr->allocate(n_elements * sizeof(T)));
}

template <typename T, bool detailed> void allocator_base<T, detailed>::
deallocate(pointer p, size_type n_elements) {
    QALLOC_ASSERT(n_elements > 0);
    if (p == nullptr) return;
    QALLOC_IF_CONSTEXPR(detailed) {
        m_pool_ptr->template detailed_deallocate<T>(qalloc::pointer::launder(reinterpret_cast<byte_pointer>(p)), n_elements * sizeof(T));
    }
    else {
        m_pool_ptr->deallocate(qalloc::pointer::launder(reinterpret_cast<byte_pointer>(p)), n_elements * sizeof(T));
    }
}

template <typename T, bool detailed>
constexpr pool_pointer allocator_base<T, detailed>::pool() const noexcept {
    return m_pool_ptr;
}

template <typename T, bool T_detailed, typename U, bool U_detailed>
constexpr bool operator==(const allocator_base<T, T_detailed>&, const allocator_base<U, U_detailed>&) noexcept {
    return false;
}

template <typename T, bool T_detailed, typename U, bool U_detailed>
constexpr bool operator!=(const allocator_base<T, T_detailed>&, const allocator_base<U, U_detailed>&) noexcept {
    return true;
}
QALLOC_END

#endif // QALLOC_ALLOCATOR_IMPL_HPP