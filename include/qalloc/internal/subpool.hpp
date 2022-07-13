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

/// @file qalloc/internal/subpool.hpp
/// @brief qalloc sub pool class header file.
/// @author yusing
/// @date 2022-07-04

#ifndef QALLOC_SUBPOOL_HPP
#define QALLOC_SUBPOOL_HPP

#include <atomic>
#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/pointer.hpp>
#include <qalloc/internal/memory.hpp>

QALLOC_BEGIN
/// @brief qalloc subpool class.
class subpool_t { // use byte pointer for easier pointer arithmetics
public:
    explicit inline
    subpool_t(size_t n_bytes) noexcept
        : m_pos(static_cast<byte_ptr_t>(q_malloc(n_bytes)))
        , m_begin(m_pos.load())
        , m_end(m_begin + n_bytes) {}

    QALLOC_NODISCARD constexpr const_byte_ptr_t
    begin() const noexcept { return m_begin; }

    QALLOC_NODISCARD constexpr const_byte_ptr_t
    end() const noexcept { return m_end; }

    QALLOC_NODISCARD inline byte_ptr_t
    pos() const noexcept { return m_pos.load(std::memory_order_relaxed); }

    inline auto
    size_left() const noexcept {
        return m_end - pos();
    }

    QALLOC_NODISCARD constexpr auto
    size() const noexcept {
        return m_end - m_begin;
    }

    inline void
    advance(ptrdiff_t n_bytes) noexcept {
        QALLOC_ASSERT(m_pos + n_bytes <= m_end);
        m_pos.fetch_add(n_bytes, std::memory_order_relaxed);
    }

    inline void
    advance_end() noexcept {
        m_pos.fetch_add(size_left(), std::memory_order_relaxed);
    }

    inline void
    release() noexcept {
        QALLOC_ASSERT(m_pos == m_end);
        q_free(pos());
        m_pos.store(nullptr, std::memory_order_relaxed);
        m_begin = nullptr;
        m_end = nullptr;
    }

    QALLOC_NODISCARD inline bool
    can_advance(size_t n_bytes) const noexcept {
        return pos() + n_bytes <= m_end;
    }

    QALLOC_NODISCARD inline bool
    is_valid(byte_ptr_t p) const noexcept {
        return p >= m_begin && p < m_end;
    }

    void* operator new(size_t n) {
        return q_malloc(n);
    }

    void operator delete (void* p) {
        q_free(p);
    }
private:
    std::atomic<byte_ptr_t>  m_pos;
    const_byte_ptr_t         m_begin;
    const_byte_ptr_t         m_end;
}; // struct subpool_t
using subpool_ptr_t = subpool_t*;

QALLOC_END

#endif //QALLOC_SUBPOOL_HPP
