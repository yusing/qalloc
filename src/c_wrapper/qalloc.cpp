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

/// @file qalloc.cpp
/// @brief qalloc c wrapper library implementation file.
/// @author yusing
/// @date 2022-07-07

#include <mutex>
#include <utility>
#include <cstring>
#include <qalloc/qalloc.h>
#include <qalloc/internal/pool.hpp>
#include <qalloc/internal/pool_impl.hpp>
#include <qalloc/internal/pool_base_impl.hpp>
#include <qalloc/internal/global_pool.hpp>
#include <qalloc/internal/pointer.hpp>

using namespace qalloc;

extern "C" {

/// @brief a wrapper struct for q_allocate to store allocated size.
struct c_block_info_t {
    /// @brief allocated size.
    size_type size;

    static constexpr c_block_info_t* of(void_pointer p) noexcept {
        return pointer::sub<c_block_info_t*>(p, sizeof(c_block_info_t));
    }
};

QALLOC_EXPORT void* q_allocate(size_t size) {
    auto* p = internal::get_pool<int>()->detailed_allocate<int>(size);
    new (pointer::launder(p)) c_block_info_t{size};
    return p + sizeof(c_block_info_t);
}

QALLOC_EXPORT void* q_callocate(size_t n, size_t size) {
    n *= size;
    return std::memset(q_allocate(n), 0, n);
}

QALLOC_EXPORT void q_deallocate(void* ptr) {
    auto* block_p = c_block_info_t::of(ptr);
    internal::get_pool<int>()->detailed_deallocate<int>(
        reinterpret_cast<byte_pointer>(block_p),
        block_p->size
    );
}

QALLOC_EXPORT void* q_reallocate(void* ptr, size_t size) {
    auto* block_p = c_block_info_t::of(ptr);
    size_t old_size = block_p->size;
    auto* new_p = q_allocate(size);
    std::memcpy(new_p, ptr, std::min(size, old_size));
    q_deallocate(block_p);
    return new_p;
}

QALLOC_EXPORT size_t q_garbage_collect() {
    return internal::get_pool<int>()->gc();
}

}