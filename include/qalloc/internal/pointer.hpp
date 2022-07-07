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

/// @file qalloc/internal/pointer.hpp
/// @brief qalloc pointer utilities header file.
/// @author yusing
/// @date 2022-07-02

#ifndef QALLOC_POINTER_HPP
#define QALLOC_POINTER_HPP

#include <ostream>
#include <utility>
#include <qalloc/internal/defs.hpp>

QALLOC_BEGIN
/// @brief single byte enum type
enum class byte : unsigned char{};

using byte_pointer        = byte*;
using const_byte_pointer  = const byte*;
using void_pointer        = void*;
using const_void_pointer  = const void*;
using size_type           = std::size_t;
using difference_type     = std::ptrdiff_t;

/// @brief size_type enum type for index
enum class index_type : size_type {
    Zero = 0
};

/// @brief std::ostream input operator for constant byte pointer
/// @param os std::ostream object
/// @param p constant byte pointer
/// @return @b os
std::ostream& operator<<(std::ostream& os, qalloc::const_byte_pointer p) {
    os << static_cast<qalloc::const_void_pointer>(p);
    return os;
}

/// @brief index_type decrement operator
/// @param i index_type
/// @return decremented index reference
constexpr index_type& operator--(index_type& i) {
    return i = static_cast<index_type>(static_cast<size_type>(i) - 1);
}

/// @brief size_type literal suffix operator
/// @return size_type representation of the literal
constexpr size_type operator "" _z (unsigned long long n) {
    return static_cast<size_type>(n);
}
static_assert(sizeof(byte) == 1_z, "byte is not 1 byte");
static_assert(sizeof(index_type) == sizeof(size_type), "size of index_type != size of size_type!");

/// @brief cast from index_type to size_type
/// @return size_type representation of the index_type
constexpr size_type size_cast(index_type i) {
    return static_cast<size_type>(i);
}

/// @brief cast from difference_type to size_type
/// @return size_type representation of the difference_type
constexpr size_type size_cast(difference_type diff) {
    return static_cast<size_type>(diff);
}

/// @brief pointer utilities namespace
namespace pointer {

template <typename T>
/// @brief std::launder wrapper
/// @tparam T type of the pointer
/// @param p pointer to be laundered
/// @return laundered pointer
constexpr T* launder(T* p) {
#if QALLOC_CXX_17
    if constexpr(!std::is_void_v<T>) {
        return std::launder(p);
    }
    return p;
#else
    return p;
#endif
}

template <typename OutType = byte_pointer, typename OffsetType, typename InType>
constexpr OutType add(InType ptr, OffsetType offset) {
    static_assert(
        (std::is_const<InType>::value && std::is_const<OutType>::value)
        || (!std::is_const<InType>::value && !std::is_const<OutType>::value)
        , "constness of InType and OutType must be the same"
    );
    return static_cast<OutType>(
        static_cast<void_pointer>(
            static_cast<byte_pointer>(
                static_cast<void_pointer>(launder(ptr))
            ) + offset
        )
    );
}

template <typename OutType = byte_pointer, typename OffsetType, typename InType>
constexpr OutType sub(InType ptr, OffsetType offset) {
    static_assert(
        (std::is_const<InType>::value && std::is_const<OutType>::value)
        || (!std::is_const<InType>::value && !std::is_const<OutType>::value)
        , "constness of InType and OutType must be the same"
    );
    return static_cast<OutType>(
        static_cast<void_pointer>(
            static_cast<byte_pointer>(
                static_cast<void_pointer>(launder(ptr))
            ) - offset
        )
    );
}

constexpr bool in_range(const_void_pointer pos, const_void_pointer lb, const_void_pointer ub) {
    return pos >= lb && pos < ub;
}

constexpr byte_pointer remove_const(const_byte_pointer p) {
    return const_cast<byte_pointer>(launder(p));
}

} // namespace pointer
QALLOC_END

#endif