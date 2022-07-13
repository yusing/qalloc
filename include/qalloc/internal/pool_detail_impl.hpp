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

/// @file qalloc/internal/pool_impl.hpp
/// @brief qalloc pool class implementation header file.
/// @author yusing
/// @date 2022-07-06

#ifndef QALLOC_POOL_DETAIL_IMPL_HPP
#define QALLOC_POOL_DETAIL_IMPL_HPP

#include <atomic> // std::atomic
#include <algorithm> // std::remove_if
#include <stdexcept> // std::bad_alloc
#include <iostream> // std::cout, std::endl
#include <qalloc/internal/pool.hpp>
#include <qalloc/internal/pool_impl.hpp>
#include <qalloc/internal/debug_log.hpp>
#include <qalloc/internal/memory.hpp>
#include <qalloc/internal/defs.hpp>

QALLOC_BEGIN

template <class T>
byte_ptr_t pool_t::detailed_allocate(size_t n_bytes_requested) const {
    byte_ptr_t ptr = allocate(n_bytes_requested + sizeof(block_info_t));
    new (pointer::launder(ptr)) block_info_t{&typeid(T), index_t(m_subpools.size() - 1)};
    return ptr + sizeof(block_info_t);
}

template <class T>
void pool_t::detailed_deallocate(byte_ptr_t p, size_t n_bytes_requested) const {
    QALLOC_DEBUG_STATEMENT(
        auto& block = *block_info_t::of(p);
        QALLOC_ASSERT(size_cast(block.subpool_index) < m_subpools.size());
        QALLOC_ASSERT(*block.type_info == typeid(T));
    )
    deallocate(pointer::launder(p - sizeof(block_info_t)), n_bytes_requested + sizeof(block_info_t));
}

size_t pool_t::gc() const {
    pool_lock_t lock(this);
    size_t memory_freed = 0;
    for (auto it = m_freed_blocks.begin(); it != m_freed_blocks.end();) {
        auto& block = *it;
        block_info_t* block_info = block_info_t::at(block.address);
        if (size_cast(block_info->subpool_index) >= m_subpools.size()) {
            // the header of the block maybe overwritten by a reuse of block
            continue;
        }
        subpool_ptr_t owner = m_subpools[size_cast(block_info->subpool_index)];
        auto owner_size = owner->size();
        if (block.n_bytes == owner_size) { // whole subpool is freed
            // To ensure the subpool index in all blocks are valid
            // do not erase the released subpool
            debug_log("[gc]: subpool %zu released (%zu bytes)\n", size_cast(block_info->subpool_index) + 1, size_cast(owner_size));
            memory_freed += owner_size;
            // release the subpool
            owner->release(); // TODO: reuse the memory
            // update total pool size
            m_pool_total -= owner_size;
            // remove the freed block from the list
            it = m_freed_blocks.erase(it);
        }
        else {
            ++it;
        }
    }
    return memory_freed;
}

/// @brief Get an instance of pool_base_t for each type.
/// @tparam T The type of the pool.
/// @return Pointer to the instance of pool_base_t of type T.
template <typename T>
const pool_t& pool_t::get_instance() {
    //
    // Using different pool for different type can reduce memory fragmentation.
    // Might slightly improve performance.
    //
    static std::atomic<pool_ptr_t> instance{nullptr};

    pool_ptr_t pool_ptr = instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (pool_ptr == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock_guard(mutex);
        pool_ptr = instance.load(std::memory_order_relaxed);
        if (pool_ptr == nullptr) { // double-check
            pool_ptr = new pool_t(1048576_z);
            std::atomic_thread_fence(std::memory_order_release);
            instance.store(pool_ptr, std::memory_order_relaxed);
        }
    }
    return *pool_ptr;
}

QALLOC_END

#endif