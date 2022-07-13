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

#ifndef QALLOC_POOL_HPP
#define QALLOC_POOL_HPP

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <qalloc/internal/pointer.hpp>
#include <qalloc/internal/block.hpp>
#include <qalloc/internal/optional.hpp>

QALLOC_BEGIN

/// @brief qalloc pool base class.
class pool_t {
    enum class can_merge_t { No, Yes };
public:
    pool_t() = delete;
    explicit pool_t(size_t byte_size);
    pool_t(const pool_t&) = delete;
    pool_t(pool_t&&) = delete;
    pool_t& operator=(const pool_t&) = delete;
    pool_t& operator=(pool_t&&) = delete;
    virtual ~pool_t();

    byte_ptr_t
    allocate(size_t n_bytes) const;

    template <can_merge_t can_merge = can_merge_t::Yes> void
    deallocate(byte_ptr_t p, size_t n_bytes) const;

    constexpr size_t
    pool_size() const noexcept;

    size_t
    bytes_used() const noexcept;

    template <class T> byte_ptr_t
    detailed_allocate(size_t n_bytes_requested) const;

    template <class T> void
    detailed_deallocate(byte_ptr_t p, size_t n_bytes_requested) const;

    size_t
    gc() const;

    template <typename T> static const pool_t&
    get_instance();

    QALLOC_MALLOC_FUNCTION(void_ptr_t
    operator new(size_t));

    void
    operator delete(QALLOC_RESTRICT void_ptr_t p);

    void
    print_info(bool usage_only = false) const;


protected:
    class pool_lock_t;

    mutable std::vector<subpool_ptr_t>  m_subpools;         // linked list of subpools
    mutable std::atomic<subpool_ptr_t>  m_cur_subpool;      // pointer to current subpool
    mutable std::vector<freed_block_t>  m_freed_blocks;     // vector of freed blocks
    mutable size_t                      m_pool_total;       // sum of all subpools' sizes
    mutable optional<std::thread::id>   m_current_thread;   // thread id of the thread that locked the pool
    mutable std::mutex                  m_mutex;            // mutex for locking the pool
    mutable std::condition_variable     m_cv;               // condition variable for thread waiting

    bool is_valid(void_ptr_t p) const;
    void add_subpool(size_t n_bytes) const;
}; // class pool_base_t

using pool_ptr_t = const pool_t*;
QALLOC_END

#endif //QALLOC_POOL_HPP