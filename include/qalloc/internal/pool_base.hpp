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

/// @file qalloc/internal/pool_base.hpp
/// @brief qalloc pool base class header file.
/// @author yusing
/// @date 2022-07-06

#ifndef QALLOC_POOL_BASE_HPP
#define QALLOC_POOL_BASE_HPP

#include <list>
#include <vector>
#include <qalloc/internal/pointer.hpp>
#include <qalloc/internal/block.hpp>

QALLOC_BEGIN

/// @brief qalloc pool base class.
class pool_base_t {
public:
    pool_base_t() = delete;
    explicit pool_base_t(size_type byte_size);
    pool_base_t(const pool_base_t&) = delete;
    pool_base_t(pool_base_t&&) = delete;
    pool_base_t& operator=(const pool_base_t&) = delete;
    pool_base_t& operator=(pool_base_t&&) = delete;
    virtual ~pool_base_t();

    byte_pointer allocate(size_type n_bytes) const;
    template <bool merge = true>
    void deallocate(byte_pointer p, size_type n_bytes) const;

    constexpr size_type pool_size() const noexcept;
    size_type bytes_used() const noexcept;

    QALLOC_MALLOC_FUNCTION(void_pointer operator new(size_type));
    void operator delete(QALLOC_RESTRICT void_pointer p);

    // debugging
    void print_info(bool usage_only = false) const;
protected:
    mutable std::vector<subpool_t>      m_subpools;       // linked list of subpools
    mutable subpool_t*                  m_cur_subpool;    // pointer to current subpool
    mutable std::vector<freed_block_t>  m_freed_blocks;   // vector of freed blocks
    mutable size_type                   m_pool_total;     // sum of all subpools' sizes
    static subpool_t new_subpool(size_type n_bytes) ; // n_bytes >= 1
    bool is_valid(void_pointer p) const noexcept;
    void add_subpool(size_type n_bytes) const;

    constexpr bool can_allocate(size_type n_bytes) const noexcept;
}; // class pool_base_t
QALLOC_END

#endif //QALLOC_POOL_BASE_HPP