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

using byte_ptr_t        = byte*;
using const_byte_ptr_t  = const byte*;
using void_ptr_t        = void*;
using const_void_ptr_t  = const void*;
using std::size_t;
using std::ptrdiff_t;

/// @brief size_type enum type for index
enum class index_t : size_t {};

/// @brief std::ostream input operator for constant byte pointer
/// @param os std::ostream object
/// @param p constant byte pointer
/// @return @b os
std::ostream& operator<<(std::ostream& os, qalloc::const_byte_ptr_t p) {
    os << static_cast<qalloc::const_void_ptr_t>(p);
    return os;
}

/// @brief index_type decrement operator
/// @param i index_type
/// @return decremented index reference
constexpr index_t& operator--(index_t& i) {
    return i = static_cast<index_t>(static_cast<size_t>(i) - 1);
}

/// @brief size_type literal suffix operator
/// @return size_type representation of the literal
constexpr size_t operator "" _z (unsigned long long n) {
    return static_cast<size_t>(n);
}
static_assert(sizeof(byte) == 1_z, "byte is not 1 byte");
static_assert(sizeof(index_t) == sizeof(size_t), "size of index_type != size of size_type!");

/// @brief cast from index_type to size_type
/// @return size_type representation of the index_type
constexpr size_t size_cast(index_t i) {
    return static_cast<size_t>(i);
}

/// @brief cast from difference_type to size_type
/// @return size_type representation of the difference_type
constexpr size_t size_cast(ptrdiff_t diff) {
    return static_cast<size_t>(diff);
}

/// @brief pointer utilities namespace
namespace pointer {

#if QALLOC_CXX_17
    using std::launder;
#else  // QALLOC_CXX_17
    template <typename T>
    constexpr T* launder(T* p) {
        return p;
    }
#endif // QALLOC_CXX_17

template <typename OutType = byte_ptr_t, typename OffsetType, typename InType>
constexpr OutType add(InType ptr, OffsetType offset) {
    static_assert(
        (std::is_const<InType>::value && std::is_const<OutType>::value)
        || (!std::is_const<InType>::value && !std::is_const<OutType>::value)
        , "constness of InType and OutType must be the same"
    );
    return static_cast<OutType>(
        static_cast<void_ptr_t>(
            static_cast<byte_ptr_t>(
                static_cast<void_ptr_t>(ptr)
            ) + offset
        )
    );
}

template <typename OutType = byte_ptr_t, typename OffsetType, typename InType>
constexpr OutType sub(InType ptr, OffsetType offset) {
    static_assert(
        (std::is_const<InType>::value && std::is_const<OutType>::value)
        || (!std::is_const<InType>::value && !std::is_const<OutType>::value)
        , "constness of InType and OutType must be the same"
    );
    return static_cast<OutType>(
        static_cast<void_ptr_t>(
            static_cast<byte_ptr_t>(
                static_cast<void_ptr_t>(ptr)
            ) - offset
        )
    );
}

constexpr bool in_range(const_void_ptr_t pos, const_void_ptr_t lb, const_void_ptr_t ub) {
    return pos >= lb && pos < ub;
}

constexpr byte_ptr_t remove_const(const_byte_ptr_t p) {
    return const_cast<byte_ptr_t>(p);
}

} // namespace pointer
QALLOC_END

#endif