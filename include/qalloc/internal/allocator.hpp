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

/// @file qalloc/internal/allocator.hpp
/// @brief qalloc allocator class header file.
/// @author yusing
/// @date 2022-07-02

#ifndef QALLOC_ALLOCATOR_HPP
#define QALLOC_ALLOCATOR_HPP

#include <cstddef> // std::ptrdiff_t
#include <qalloc/internal/pool.hpp>

QALLOC_BEGIN

/// @brief qalloc allocator base class.
/// @tparam T The type of the object to allocate.
/// @tparam detailed Whether to store allocation details (i.e. type info).
template <typename T, bool detailed>
class allocator_base {
public:
    using value_type       = T;
    using pointer          = T*;
    using const_pointer    = const T*;
    using reference        = T&;
    using const_reference  = const T&;
    using size_type        = qalloc::size_type;
    using difference_type  = std::ptrdiff_t;
    using is_always_equal  = std::false_type;

    template <typename U>
    class rebind {
    public:
        using other = allocator_base<U, detailed>;
    };

    allocator_base() noexcept;
    explicit allocator_base(pool_pointer) noexcept;
    allocator_base(const allocator_base&) noexcept;
    template <typename U, bool U_detailed>
    explicit allocator_base(const allocator_base<U, U_detailed>&) noexcept;
    allocator_base& operator=(const allocator_base&) noexcept;

    virtual pointer allocate(size_type n_elements);
    virtual void deallocate(pointer p, size_type n_elements);

    QALLOC_NODISCARD
    constexpr pool_pointer pool() const noexcept;

private:
    pool_pointer m_pool_ptr;
}; // class allocator

/// @brief qalloc allocator class with typeinfo and gc support.
template <typename T>
using allocator = allocator_base<T, true>;

/// @brief qalloc simple allocator class without typeinfo and gc support.
template <typename T>
using simple_allocator = allocator_base<T, false>;

QALLOC_END

#endif // QALLOC_ALLOCATOR_HPP