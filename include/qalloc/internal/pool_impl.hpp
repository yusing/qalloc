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

#ifndef QALLOC_POOL_IMPL_HPP
#define QALLOC_POOL_IMPL_HPP

#include <algorithm> // std::find_if
#include <iterator>
#include <qalloc/internal/pool.hpp>
#include <qalloc/internal/debug_log.hpp>
#include <qalloc/internal/memory.hpp>
#include <qalloc/internal/type_info.hpp>

QALLOC_BEGIN
static std::atomic<std::size_t> thread_counter = 0;

inline std::size_t thread_id() {
    thread_local std::size_t tid = thread_counter++;
    return tid;
}

class pool_t::pool_lock_t {
public:
    explicit pool_lock_t(pool_ptr_t pool) : m_pool(pool) {
        /// do not use mutex lock when there is only one thread
        /// huge performance boost
        if (thread_id() == 0) {
            m_unlock_on_exit = false;
            return;
        }
        std::unique_lock<std::mutex> lock(pool->m_mutex);
        pool->m_cv.wait(lock, [pool]() { return !pool->m_current_thread.has_value() || pool->m_current_thread == std::this_thread::get_id(); });
        QALLOC_ASSERT(!pool->m_current_thread.has_value() || pool->m_current_thread == std::this_thread::get_id());
        if (pool->m_current_thread.has_value()) {
            // other function in the same thread already locked this pool
            m_unlock_on_exit = false;
        } else {
            pool->m_current_thread = std::this_thread::get_id();
            m_unlock_on_exit = true;
        }
    }
    ~pool_lock_t() {
        if (m_unlock_on_exit) {
            std::unique_lock<std::mutex> lock(m_pool->m_mutex);
            QALLOC_ASSERT(m_pool->m_current_thread.has_value());
            QALLOC_ASSERT(m_pool->m_current_thread == std::this_thread::get_id());
            m_pool->m_current_thread = QALLOC_OPT_NULL;
            m_pool->m_cv.notify_one();
        }
    }
private:
    pool_ptr_t m_pool;
    bool m_unlock_on_exit;
};

inline pool_t::pool_t(size_t byte_size) :
    m_subpools        (1, new subpool_t(byte_size)),
    m_cur_subpool     (m_subpools.front()),
    m_freed_blocks    (),
    m_pool_total      (byte_size),
    m_current_thread  ()
{
    QALLOC_ASSERT(byte_size > 0);
    QALLOC_ASSERT(!m_subpools.empty());
    QALLOC_ASSERT(m_cur_subpool != nullptr);
    debug_log("[pool] pool of %zu bytes constructed\n", byte_size);
}

inline pool_t::~pool_t() {
    pool_lock_t lock(this);
    debug_log("%s\n", "[pool] pool destructed");
    QALLOC_DEBUG_STATEMENT(print_info(true);)
    for (subpool_ptr_t subpool : m_subpools) {
        subpool->release();
        delete subpool;
    }
}

inline byte_ptr_t pool_t::allocate(size_t n_bytes) const {
    pool_lock_t lock(this);

    QALLOC_ASSERT(n_bytes > 0);
    subpool_ptr_t cur_subpool = m_cur_subpool.load(std::memory_order_relaxed);
    // if current pool cannot allocate n_bytes
    // no need to check the freed blocks (assumed they are smaller than n_bytes)
    if (cur_subpool->can_advance(n_bytes)) {
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
                size_t size_left = reused_block.n_bytes - n_bytes;
                deallocate<can_merge_t::Yes>(reused_block.address, size_left);
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
        add_subpool(std::min(n_bytes * 2, size_cast(cur_subpool->size() * 2)));
        cur_subpool = m_cur_subpool.load(std::memory_order_relaxed);
    }

    byte_ptr_t address = pointer::launder(cur_subpool->pos());
    QALLOC_ASSERT(address != nullptr);

    // advance the current pointer
    cur_subpool->advance(static_cast<ptrdiff_t>(n_bytes));
    debug_log("[allocate] allocated %zu bytes @ %p (Thread %zu Subpool %zu)\n", n_bytes, address, thread_id(),
                m_subpools.size());
    return address;
}

template <pool_t::can_merge_t can_merge>
inline void pool_t::deallocate(byte_ptr_t p, size_t n_bytes) const {
    pool_lock_t lock(this);

    QALLOC_ASSERT(p != nullptr);
    QALLOC_ASSERT(n_bytes > 0);
    QALLOC_ASSERT(is_valid(p));

    if (m_freed_blocks.empty()) {
        m_freed_blocks.emplace_back(freed_block_t{n_bytes, p});
    } else {
        freed_block_t freed_block{n_bytes, p};
        // make sure the freed block is sorted by address in ascending order (in order to merge blocks)
        auto insert_pos = std::lower_bound(
                m_freed_blocks.begin(),
                m_freed_blocks.end(),
                freed_block,
                freed_block_t::less);

        QALLOC_IF_CONSTEXPR(can_merge == can_merge_t::Yes) { // if merge allowed
            if (insert_pos != m_freed_blocks.end() && // cannot dereference end()
                freed_block.is_adjacent_to(*insert_pos)) { // is adjacent to the next block

                debug_log("[deallocate] merged %p (%zu bytes) and %p (%zu bytes) into %zu bytes (Thread %zu)\n", p,
                          n_bytes,
                          insert_pos->address, insert_pos->n_bytes, insert_pos->n_bytes + n_bytes, thread_id());

                // merge with the next block
                insert_pos->n_bytes += n_bytes;
                insert_pos->address = p;

                if (m_freed_blocks.size() > 1_z) {
                    // iterate from the second block
                    insert_pos = std::next(m_freed_blocks.begin(), 1);
                    while (insert_pos != m_freed_blocks.end()) {
                        auto prev = std::prev(insert_pos, 1);
                        // if its adjacent to the previous block
                        if (prev->is_adjacent_to(*insert_pos)) {
                            debug_log(
                                    "[deallocate] merged %p (%zu bytes) and %p (%zu bytes) into %zu bytes (Thread %zu)\n",
                                    prev->address, prev->n_bytes, insert_pos->address, insert_pos->n_bytes,
                                    prev->n_bytes + insert_pos->n_bytes, thread_id());
                            // merge with the previous block
                            prev->n_bytes += insert_pos->n_bytes;
                            // remove the current block
                            insert_pos = m_freed_blocks.erase(insert_pos);
                        } else {
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

inline void pool_t::add_subpool(size_t n_bytes) const {
    pool_lock_t lock(this);

    subpool_ptr_t cur_subpool = m_cur_subpool.load(std::memory_order_relaxed);

    debug_log("[allocate] adding new subpool with size %zu (Thread %zu Subpool %zu)\n", n_bytes, thread_id(),
                m_subpools.size());
    // if there is space left in current subpool
    size_t size_left = cur_subpool->size_left();
    if (size_left != 0) {
        byte_ptr_t pos = cur_subpool->pos();
        debug_log("[allocate] subpool %zu has %zu bytes left @ %p (Thread %zu)\n", m_subpools.size(),
                    size_left, pos, thread_id());
        // advance current subpool to the end
        cur_subpool->advance_end();
        // mark it as freed
        deallocate<can_merge_t::No>(pos, size_left);
    }
    // add new subpool of n_bytes size
    // then update current subpool
#if QALLOC_CXX_17
    m_cur_subpool.store(
        m_subpools.emplace_back(new subpool_t(n_bytes))
    );
#else // QALLOC_CXX_17
    m_subpools.emplace_back(new subpool_t(n_bytes));
    m_cur_subpool.store(m_subpools.back());
#endif // QALLOC_CXX_17
    // update total pool size
    m_pool_total += n_bytes;
}

size_t pool_t::bytes_used() const noexcept {
    pool_lock_t lock(this);

    // take total pool size and subtract each size of freed blocks
    size_t bytes_used = m_pool_total;
    for (const auto& block : m_freed_blocks) {
        bytes_used -= block.n_bytes;
    }
    // and also subtract the unused bytes in current subpool
    bytes_used -= size_cast(m_cur_subpool.load()->size_left());
    return bytes_used;
}

constexpr size_t pool_t::pool_size() const noexcept {
    return m_pool_total;
}

bool pool_t::is_valid(void_ptr_t p) const {
    pool_lock_t lock(this);
    // check if the pointer is valid in any subpool
    return std::any_of(
        m_subpools.begin(),
        m_subpools.end(),
        [p](subpool_ptr_t subpool) {
            return subpool != nullptr && subpool->is_valid(static_cast<byte_ptr_t>(p));
        }
    );
}

QALLOC_MALLOC_FUNCTION(void_ptr_t
pool_t::operator new(size_t n_bytes)) {
    return q_malloc(n_bytes);
}

void pool_t::operator delete(QALLOC_RESTRICT void_ptr_t p) {
    q_free(p);
}

/// @brief to print out all subpools and freed blocks
inline void pool_t::print_info(bool usage_only) const {
    pool_lock_t lock(this);

    QALLOC_PRINT("Memory Pool:");
    size_t bytes_used = this->bytes_used();
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
    for (subpool_t* pool : m_subpools) {
        if (pool == nullptr) {
            QALLOC_PRINTF("    %d: released by gc\n", i);
        }
        else {
            QALLOC_PRINTF("    %d: %p ~ %p (%zu bytes)\n", i, pool->begin(), pool->end(), size_cast(pool->size()));
            QALLOC_PRINTF("      Position @ %p\n", pool->pos());
        }
        ++i;
    }
    auto& last_subpool = m_subpools.back();
    // last_subpool cannot be nullptr, because it is always added when the pool is full
    if (last_subpool->size_left() != 0) {
        QALLOC_PRINTF("      %zu bytes unused\n", size_cast(last_subpool->size_left()));
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

#endif //QALLOC_POOL_IMPL_HPP
