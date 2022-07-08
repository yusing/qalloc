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

/// @file qalloc/internal/global_pool.hpp
/// @brief qalloc global pool implementation header file.
/// @author yusing
/// @date 2022-07-04

#ifndef QALLOC_GLOBAL_POOL_HPP
#define QALLOC_GLOBAL_POOL_HPP

#include <list>
#include <string>
#include <utility> // std::pair
#include <type_traits>  // std::is_scalar, std::integral_constant, std::void_t, std::false_type, std::true_type
#include <mutex> // std::mutex, std::lock_guard
#include <atomic> // std::atomic
#include <qalloc/internal/allocator.hpp>
#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/memory.hpp>

QALLOC_INTERNAL_BEGIN
/// @internal
template <size_type POOL_SIZE>
inline pool_pointer initialize_pool_if_needed(std::atomic<pool_pointer>& pool_atomic) {
    static std::mutex g_pool_mutex;
    pool_pointer p_pool = pool_atomic.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (p_pool == nullptr) {
        std::lock_guard<std::mutex> lock_guard(g_pool_mutex);
        p_pool = pool_atomic.load(std::memory_order_relaxed);
        if (p_pool == nullptr) { // double-check
            p_pool = new pool_t(POOL_SIZE);
            atomic_thread_fence(std::memory_order_release);
            pool_atomic.store(p_pool, std::memory_order_relaxed);
        }
    }
    return p_pool;
}

#if QALLOC_CXX_14
/*
 * Using different pool for different type can reduce memory fragmentation.
 * Might slightly improve performance.
 */
    thread_local std::atomic<pool_pointer> g_pool_shared;

    template <typename T>
    thread_local std::atomic<pool_pointer> g_pool_unique;

    template <typename T>
    pool_pointer get_pool() {
#if QALLOC_DEBUG
        // use shared pool for debug mode
        return initialize_pool_if_needed<128>(g_pool_shared);
#else // QALLOC_DEBUG
        return initialize_pool_if_needed<sizeof(T) * 16_z>(g_pool_unique<T>); // the initial size does not affect performance much, just keep it small
#endif // QALLOC_DEBUG
    }
#else // QALLOC_CXX_14
    // use ahared pool for all types in C++11 or earlier, since variable templates is not supported
    template <typename>
    pool_pointer get_pool() {
        return initialize_pool_if_needed<128_z>(g_pool_shared);
    }
#endif // QALLOC_CXX_14

QALLOC_INTERNAL_END
#endif // QALLOC_GLOBAL_POOL_HPP
