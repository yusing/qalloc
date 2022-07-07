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
#include <qalloc/qalloc.h>
#include <qalloc/internal/pool.hpp>
#include <qalloc/internal/pool_impl.hpp>
#include <qalloc/internal/pool_base_impl.hpp>
#include <qalloc/internal/global_pool.hpp>
using namespace qalloc;

/// @brief a wrapper struct for q_allocate to store allocated size.
struct c_block_info_t {
    /// @brief allocated size.
    size_type size;
};

QALLOC_EXPORT void* q_allocate(size_t size) {
    auto* p = internal::get_pool<int>()->detailed_allocate<void>(size);
    new (pointer::launder(p)) c_block_info_t{size};
    return p + sizeof(c_block_info_t);
}

QALLOC_EXPORT void q_deallocate(void* ptr) {
    auto* byte_p = static_cast<byte_pointer>(ptr);
    auto* block_p = static_cast<c_block_info_t*>(ptr);
    internal::get_pool<int>()->detailed_deallocate<void>(
        pointer::launder(byte_p - sizeof(c_block_info_t)),
        block_p->size
    );
}

QALLOC_EXPORT size_t q_garbage_collect() {
    return internal::get_pool<int>()->gc();
}