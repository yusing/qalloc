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

#ifndef QALLOC_POOL_IMPL_HPP
#define QALLOC_POOL_IMPL_HPP

#include <algorithm> // std::remove_if
#include <stdexcept> // std::bad_alloc
#include <iostream> // std::cout, std::endl
#include <qalloc/internal/pool_base.hpp>
#include <qalloc/internal/debug_log.hpp>
#include <qalloc/internal/memory.hpp>
#include <qalloc/internal/defs.hpp>

QALLOC_BEGIN

template <class T>
byte_pointer pool_t::detailed_allocate(size_type n_bytes_requested) const {
    byte_pointer ptr = pool_base_t::allocate(n_bytes_requested + sizeof(block_info_t));
    new (pointer::launder(ptr)) block_info_t{&typeid(T), index_type(m_subpools.size() - 1)};
    return ptr + sizeof(block_info_t);
}

template <class T>
void pool_t::detailed_deallocate(byte_pointer p, size_type n_bytes_requested) const {
    QALLOC_DEBUG_STATEMENT(
        auto& block = *block_info_t::of(p);
        QALLOC_ASSERT(size_cast(block.subpool_index) < m_subpools.size());
        QALLOC_ASSERT(*block.type_info == typeid(T));
    )
    pool_base_t::deallocate(pointer::launder(p - sizeof(block_info_t)), n_bytes_requested + sizeof(block_info_t));
}

QALLOC_MAYBE_UNUSED
size_type pool_t::gc() const { // TODO: fix this, not gc triggers
    size_type memory_freed = 0;
    for (auto it = m_freed_blocks.begin(); it != m_freed_blocks.end();) {
        auto& block = *it;
        block_info_t* block_info = block_info_t::at(block.address);
        if (size_cast(block_info->subpool_index) >= m_subpools.size()) {
            // the header of the block maybe overwritten by a reuse of block
            continue;
        }
        subpool_t& owner = m_subpools[size_cast(block_info->subpool_index)];
        if (block.n_bytes == owner.size) { // whole subpool is freed
            // To ensure the subpool index in all blocks are valid
            // do not erase the released subpool
            debug_log("[gc]: subpool %zu released (%zu bytes)\n", size_cast(block_info->subpool_index) + 1, owner.size);
            memory_freed += owner.size;
            // release the subpool
            q_free(pointer::remove_const(owner.begin)); // TODO: reuse the memory
            // update total pool size
            this->m_pool_total -= owner.size;
            // reset the subpool to ZEROs/NULLs
            owner = {};
            // remove the freed block from the list
            it = m_freed_blocks.erase(it);
        }
        else {
            ++it;
        }
    }
    return memory_freed;
}
QALLOC_END

#endif