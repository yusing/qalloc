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

/// @file qalloc/internal/pool_base_impl.hpp
/// @brief qalloc pool base class implementation header file.
/// @author yusing
/// @date 2022-07-06

#ifndef QALLOC_POOL_BASE_IMPL_HPP
#define QALLOC_POOL_BASE_IMPL_HPP

#include <algorithm> // std::find_if
#include <qalloc/internal/pool_base.hpp>
#include <qalloc/internal/debug_log.hpp>
#include <qalloc/internal/memory.hpp>
#include <qalloc/internal/type_info.hpp>

QALLOC_BEGIN

inline pool_base_t::pool_base_t(size_type byte_size)
    : m_subpools       (1_z, new_subpool(byte_size)),
      m_cur_subpool    (&m_subpools.front()),
      m_freed_blocks   (),
      m_pool_total     (byte_size)
{
    QALLOC_ASSERT(byte_size > 0);
    QALLOC_ASSERT(!m_subpools.empty());
    QALLOC_ASSERT(m_cur_subpool != nullptr);
    debug_log("[pool] pool of %zu bytes constructed\n", byte_size);
}

inline pool_base_t::~pool_base_t() {
    debug_log("%s\n", "[pool] pool destructed");
    QALLOC_DEBUG_STATEMENT(print_info(true);)
    for (const auto& subpool : m_subpools) {
        q_free(pointer::remove_const(subpool.begin));
    }
}

inline subpool_t pool_base_t::new_subpool(size_type n_bytes) {
    QALLOC_RESTRICT byte_pointer begin = static_cast<byte_pointer>(q_malloc(n_bytes)); // NOLINT(modernize-use-auto)
    QALLOC_RESTRICT byte_pointer end   = begin + n_bytes;
    return subpool_t{
        begin,
        end,
        begin,  // current position
        n_bytes // subpool size
    };
}

inline byte_pointer pool_base_t::allocate(size_type n_bytes) const {
    QALLOC_ASSERT(n_bytes > 0);
    // if current pool cannot allocate n_bytes
    // no need to check the freed blocks (assumed they are smaller than n_bytes)
    if (can_allocate(n_bytes)) {
        // try to find a q_free block in the freed blocks list
        auto it = std::find_if(
                m_freed_blocks.begin(),
                m_freed_blocks.end(),
                [n_bytes](const freed_block_t& block) { // find a block with enough space
                    return block.n_bytes >= n_bytes;
                }
        );
        if (it != m_freed_blocks.end()) { // found a block with enough space

            QALLOC_ASSERT(it->n_bytes != 0);
            QALLOC_ASSERT(it->address != nullptr);
            freed_block_t reused_block = *it;
            m_freed_blocks.erase(it);
            if (reused_block.n_bytes > n_bytes) { // split if it has extra space left
                // deallocate the first (reused_block.n_bytes - n_bytes) bytes
                size_type size_left = reused_block.n_bytes - n_bytes;
                deallocate(reused_block.address, size_left);
                // and then reuse from the last n_bytes bytes of the block
                // to keep block info (i.e. subpool index and type info) at the beginning of the block
                reused_block.address += size_left;
            }
            debug_log("[allocate] reused freed block of %zu bytes and has %zu bytes left @ %p (Thread %zu Subpool %zu)\n",
                      reused_block.n_bytes, reused_block.n_bytes - n_bytes, reused_block.address, thread_id(),
                      m_subpools.size());
            QALLOC_ASSERT(reused_block.address != nullptr);
            return reused_block.address;
        }
    }
    else {
        // memory exhausted in pool
        // add new subpool with 2 * (n_bytes || m_cur_subpool->size) (larger one)
        add_subpool(std::max(n_bytes * 2, m_cur_subpool->size * 2));
    }

    byte_pointer address = pointer::launder(m_cur_subpool->pos);
    QALLOC_ASSERT(address != nullptr);

    // move the current pointer
    m_cur_subpool->pos += n_bytes;
    debug_log("[allocate] allocated %zu bytes @ %p (Thread %zu Subpool %zu)\n", n_bytes, address, thread_id(),
              m_subpools.size());
    return address;
}

template <bool merge>
inline void pool_base_t::deallocate(byte_pointer p, size_type n_bytes) const {
    QALLOC_ASSERT(p != nullptr);
    QALLOC_ASSERT(n_bytes > 0);
    QALLOC_ASSERT(is_valid(p));

    if (m_freed_blocks.empty()) {
        m_freed_blocks.emplace_back(freed_block_t{n_bytes, p});
    }
    else {
        freed_block_t freed_block{n_bytes, p};
        // make sure the freed block is sorted by address in ascending order (in order to merge blocks)
        auto insert_pos = std::lower_bound(m_freed_blocks.begin(), m_freed_blocks.end(), freed_block, freed_block_t::less);
        QALLOC_IF_CONSTEXPR(merge) { // if merge allowed
            if (insert_pos != m_freed_blocks.end() && // cannot dereference end()
                freed_block.is_adjacent_to(*insert_pos)) { // is adjacent to the next block

                debug_log("[deallocate] merged %p (%zu bytes) and %p (%zu bytes) into %zu bytes (Thread %zu)\n", p, n_bytes,
                        insert_pos->address, insert_pos->n_bytes, insert_pos->n_bytes + n_bytes, thread_id());

                // merge with the next block
                insert_pos->n_bytes += n_bytes;
                insert_pos->address = p;

                if (m_freed_blocks.size() > 1_z) {
                    // iterate from the second block
                    insert_pos = m_freed_blocks.begin() + 1;
                    while (insert_pos != m_freed_blocks.end()) {
                        auto prev = insert_pos - 1;
                        // if its adjacent to the previous block
                        if (prev->is_adjacent_to(*insert_pos)) {
                            debug_log("[deallocate] merged %p (%zu bytes) and %p (%zu bytes) into %zu bytes (Thread %zu)\n",
                                    prev->address, prev->n_bytes, insert_pos->address, insert_pos->n_bytes,
                                    prev->n_bytes + insert_pos->n_bytes, thread_id());
                            // merge with the previous block
                            prev->n_bytes += insert_pos->n_bytes;
                            // remove the current block
                            insert_pos = m_freed_blocks.erase(insert_pos);
                        }
                        else {
                            ++insert_pos;
                        }
                    }
                }
                return;
            }
        }
        // no block to merge with, insert the freed block
        debug_log("[deallocate] deallocated %zu bytes @ %p (Thread %zu Subpool %zu)\n", n_bytes, p, thread_id(),
                  m_subpools.size());
        m_freed_blocks.emplace(insert_pos, freed_block);
    }
}

inline void pool_base_t::add_subpool(size_type n_bytes) const {
    debug_log("[allocate] adding new subpool with size %zu (Thread %zu Subpool %zu)\n", n_bytes, thread_id(),
              m_subpools.size());
    // if there is space left in current subpool
    if (m_cur_subpool->end != m_cur_subpool->pos) {
        debug_log("[allocate] subpool %zu has %zu bytes left @ %p (Thread %zu)\n", m_subpools.size(),
                  m_cur_subpool->end - m_cur_subpool->pos, m_cur_subpool->pos, thread_id());
        // mark it as freed
        deallocate<false>(m_cur_subpool->pos, size_cast(m_cur_subpool->end - m_cur_subpool->pos));
    }
    // add new subpool
    m_subpools.emplace_back(new_subpool(n_bytes));
    // update current subpool
    m_cur_subpool = &m_subpools.back();
    // update total pool size
    m_pool_total += n_bytes;
}

constexpr bool pool_base_t::can_allocate(size_type n_bytes) const noexcept {
    return m_cur_subpool->pos + n_bytes <= m_cur_subpool->end;
}

size_type pool_base_t::bytes_used() const noexcept {
    // take total pool size and subtract each size of freed blocks
    size_t bytes_used = m_pool_total;
    for (const auto& block : m_freed_blocks) {
        bytes_used -= block.n_bytes;
    }
    // and also subtract the unused bytes in current subpool
    bytes_used -= size_cast(m_cur_subpool->end - m_cur_subpool->pos);
    return bytes_used;
}

constexpr size_type pool_base_t::pool_size() const noexcept {
    return m_pool_total;
}

bool pool_base_t::is_valid(void_pointer p) const noexcept {
    // check if the pointer is in range of any subpool
    return std::any_of(
        m_subpools.begin(),
        m_subpools.end(),
        [p](const subpool_t& subpool) {
            return pointer::in_range(p, subpool.begin, subpool.end);
        }
    );
}

QALLOC_MALLOC_FUNCTION(void_pointer
pool_base_t::operator new(size_type n_bytes)) {
    return q_malloc(n_bytes);
}

void pool_base_t::operator delete(QALLOC_RESTRICT void_pointer p) {
    q_free(p);
}

/// @brief to print out all subpools and freed blocks
inline void pool_base_t::print_info(bool usage_only) const {
    QALLOC_PRINT("Memory Pool:");
    size_type bytes_used = this->bytes_used();
    QALLOC_PRINTF("  Usage: %zu of %zu bytes", bytes_used, pool_size());
    if (pool_size() != 0_z) {
        QALLOC_PRINTF(" (%zu%%)\n", bytes_used * 100 / pool_size());
    }
    else {
        QALLOC_PRINTLN("");
    }
    if (usage_only) {
        return;
    }
    QALLOC_PRINTLN("  Subpools: ");
    int i = 1;
    for (const auto& pool : m_subpools) {
        if (pool.begin == nullptr) {
            QALLOC_PRINTF("    %d: released by gc\n", i);
        }
        else {
            QALLOC_PRINTF("    %d: %p ~ %p (%zu bytes)\n", i, pool.begin, pool.end, pool.size);
            QALLOC_PRINTF("      Position @ %p\n", pool.pos);
        }
        ++i;
    }
    auto& last_subpool = m_subpools.back();
    if (last_subpool.pos != last_subpool.end) {
        QALLOC_PRINTF("      %zu bytes unused\n", size_cast(last_subpool.end - last_subpool.pos));
    }
    QALLOC_PRINTLN("\n  Deallocated blocks:");
    for (const auto& block : m_freed_blocks) {
        auto allocated_block = block_info_t::at(block.address);
        QALLOC_PRINTF("    %p: %zu bytes\n", block.address, block.n_bytes);
        if (size_cast(allocated_block->subpool_index) <= m_subpools.size()) {
            QALLOC_PRINTF("      Subpool: %zu, type: ", size_cast(allocated_block->subpool_index) + 1);
            if (allocated_block->is_valid()) {
                auto type_name = demangled_type_name_of(allocated_block->type_info->name());
                QALLOC_PRINT(type_name.c_str());
            }
            else {
                QALLOC_PRINTLN("N/A");
            }
        }
        else {
            QALLOC_PRINTLN("      Subpool: N/A, type: N/A");
        }
    }
    QALLOC_PRINTLN("");
}

QALLOC_END

#endif //QALLOC_POOL_BASE_IMPL_HPP
