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

/// @file qalloc/internal/block.hpp
/// @brief qalloc block information class header file.
/// @author yusing
/// @date 2022-07-06

#ifndef QALLOC_BLOCK_HPP
#define QALLOC_BLOCK_HPP

#include <stdexcept>   // std::logic_error
#include <type_traits> // std::forward
#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/pointer.hpp>
#include <qalloc/internal/subpool.hpp>

QALLOC_BEGIN

/// @brief freed block information class.
struct freed_block_t {
    size_t      n_bytes;
    byte_ptr_t  address;

    QALLOC_NODISCARD
    static constexpr bool less(const freed_block_t& lhs, const freed_block_t& rhs) noexcept {
        return lhs.address < rhs.address;
    }

    QALLOC_NODISCARD
    constexpr bool is_adjacent_to(const freed_block_t& other) const noexcept {
        return address + n_bytes == other.address;
    }
}; // struct freed_block_t

/// @brief block allocation information class.
struct block_info_t {
    const std::type_info* type_info; // type_info of the allocated object
    index_t               subpool_index; // index of the subpool that owns this block
    // ... (allocated content)

    QALLOC_NODISCARD
    static constexpr block_info_t* of(void_ptr_t p) {
        return pointer::sub<block_info_t*>(p, sizeof(block_info_t));
    }

    QALLOC_NODISCARD
    static constexpr block_info_t* at(void_ptr_t p) {
        return static_cast<block_info_t*>(p);
    }

    QALLOC_NODISCARD
    constexpr bool is_valid() const noexcept {
        return type_info != nullptr;
    }
}; // struct block_info_t
QALLOC_END
#endif //QALLOC_BLOCK_HPP
