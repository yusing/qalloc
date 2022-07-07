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

/// @file qalloc/internal/experimental/vector.hpp
/// @brief qalloc experimental vector class header file.
/// @author yusing
/// @date 2022-07-05

#ifndef QALLOC_VECTOR_HPP
#define QALLOC_VECTOR_HPP

#include <memory>
#include <vector>
#include <type_traits> // std::is_trivially_destructible
#include <qalloc/internal/pointer.hpp>
#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/memory.hpp>
#include <qalloc/internal/experimental/iterator.hpp>

QALLOC_INTERNAL_BEGIN

template <typename T>
void q_destroy_at(T* p) {
    QALLOC_IF_CONSTEXPR(std::is_trivially_destructible<T>::value) {
        return;
    }
    p->~T();
}

template <typename T>
class vector {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using iterator = class vector_iterator : public qalloc::internal::iterator<T> {
    public:
        using iterator_category QALLOC_MAYBE_UNUSED = std::forward_iterator_tag;
        using difference_type QALLOC_MAYBE_UNUSED = std::ptrdiff_t;
        using value_type QALLOC_MAYBE_UNUSED = T;
        using pointer = T*;
        using const_pointer QALLOC_MAYBE_UNUSED = const T*;
        using reference QALLOC_MAYBE_UNUSED = T&;
        using const_reference QALLOC_MAYBE_UNUSED = const T&;
        using qalloc::internal::iterator<T>::iterator;

        QALLOC_CONSTEXPR_14
        pointer base() const {
            return this->m_p;
        }

        QALLOC_CONSTEXPR_14
        vector_iterator& operator++() {
            QALLOC_ASSERT(this->m_p != nullptr);
            ++this->m_p;
            return *this;
        }

        QALLOC_CONSTEXPR_14
        vector_iterator operator++(int) {
            QALLOC_ASSERT(this->m_p != nullptr);
            vector_iterator it = *this;
            ++(*this);
            return it;
        }

        QALLOC_CONSTEXPR_14
        vector_iterator operator+(difference_type n) {
            QALLOC_ASSERT(this->m_p != nullptr);
            vector_iterator it = *this;
            it.m_p += n;
            return it;
        }

        QALLOC_CONSTEXPR_14
        vector_iterator operator-(difference_type n) {
            QALLOC_ASSERT(this->m_p != nullptr);
            vector_iterator it = *this;
            it.m_p -= n;
            return it;
        }

        QALLOC_CONSTEXPR_14
        difference_type operator-(vector_iterator it) const {
            QALLOC_ASSERT(this->m_p != nullptr);
            QALLOC_ASSERT(it.m_p != nullptr);
            return this->m_p - it.m_p;
        }

        QALLOC_CONSTEXPR_14
        vector_iterator& operator--() {
            QALLOC_ASSERT(this->m_p != nullptr);
            --this->m_p;
            return *this;
        }

        QALLOC_CONSTEXPR_14
        vector_iterator operator--(int) {
            QALLOC_ASSERT(this->m_p != nullptr);
            vector_iterator it = *this;
            --(*this);
            return it;
        }
    };

    vector() :
            m_data(allocate_n_elements(INITIAL_SIZE)),
            m_last(m_data),
            m_capacity(INITIAL_SIZE),
            m_size(0) {}
    vector(size_type, T&& first) :
            m_data(allocate_n_elements(INITIAL_SIZE)),
            m_last(m_data + 1),
            m_capacity(INITIAL_SIZE),
            m_size(1)
    {
        new (m_data) T(std::forward<T>(first));
    }
    vector(const vector&) = delete;
    vector(vector&&) = delete;
    vector& operator=(const vector&) = delete;
    vector& operator=(vector&&) = delete;
    // no need to support copy constructor and move constructor
    // because we don't use them
    // it is only used for pool
    // once initialized by pool, it will never be copied nor moved
    ~vector() {
        QALLOC_ASSERT(m_data + m_size == m_last);
        QALLOC_IF_CONSTEXPR(std::is_trivially_destructible<T>::value) {
            for (pointer p = m_data; p != m_last; ++p) {
                q_destroy_at(p);
            }
        }
        q_free(m_data);
    }

    template <typename ...Args>
    void emplace_back(Args&& ...args) {
        if (m_size == m_capacity) {
            m_capacity <<= 1;
            pointer  new_data = allocate_n_elements(m_capacity);
            iterator end = this->end();
            iterator old_it   = begin();
            iterator new_it   = iterator(new_data);
            while (old_it != end) {
                move(old_it++, new_it++);
            }
            q_free(m_data); // q_free old pointer
            m_data = new_data;
            m_last = m_data + m_size; // update end
        }
        new (m_last) value_type(std::forward<Args>(args)...);
        ++m_last;
        ++m_size; // increase m_size
    }

    template <typename ...Args>
    void emplace(iterator pos, Args&& ...args) {
        QALLOC_ASSERT(pos == end() || qalloc::pointer::in_range(pos, begin(), end()));
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return;
        }
        if (m_size == m_capacity) {
            m_capacity <<= 1;
            pointer new_data = allocate_n_elements(m_capacity);
            iterator old_it  = begin();
            iterator old_end = end();
            iterator new_it  = iterator(new_data);
            // copy from old_it to pos to new_it
            while (old_it != pos) {
                move(old_it++, new_it++);
            }
            QALLOC_ASSERT(new_it.base() - new_data == pos.base() - m_data);
            new (new_it.base()) value_type(std::forward<Args>(args)...); // emplace at pos
            ++new_it; // skip pos
            // copy from old_it to end to new_it
            while (old_it != old_end) {
                move(old_it++, new_it++);
            }
            ++m_size; // increase m_size
            m_data = new_data; // assign new pointer
            m_last = new_it.base(); // update end
        }
        else {
            // move from pos to end to pos + 1
            iterator old_it = end() - 1;
            iterator new_it = old_it + 1;
            while (new_it != pos) {
                move(old_it--, new_it--);
            }
            new (pos.base()) value_type(std::forward<Args>(args)...); // emplace at pos
            ++m_last;
            ++m_size; // increase m_size
        }
    }

    void erase(iterator pos) {
        QALLOC_ASSERT(m_size != 0); // can't erase from an empty vector
        QALLOC_ASSERT(pos == end() || qalloc::pointer::in_range(pos, begin(), end()));
        q_destroy_at(pos.base()); // destroy element
        iterator last = end() - 1;
        iterator next = pos + 1;
        while (pos != last) { // move elements
            move(next++, pos++); // shift left
        }
        --m_last;
        --m_size;
    }

    QALLOC_MAYBE_UNUSED QALLOC_NODISCARD
    constexpr reference front() const {
        return *m_data;
    }

    QALLOC_MAYBE_UNUSED QALLOC_NODISCARD
    constexpr reference back() const {
        assert(m_size != 0);
        return *(m_last - 1);
    }

    QALLOC_NODISCARD
    constexpr reference operator[](size_t index) const {
        QALLOC_ASSERT(index < m_size);
        return m_data[index];
    }

    QALLOC_NODISCARD
    constexpr size_type capacity() const noexcept {
        return m_capacity;
    }

    QALLOC_NODISCARD
    constexpr size_type size() const noexcept {
        return m_size;
    }

    QALLOC_NODISCARD
    constexpr bool empty() const noexcept {
        return m_size == 0_z;
    }

    QALLOC_NODISCARD QALLOC_CONSTEXPR_14
    iterator begin() noexcept {
        return iterator(m_data);
    }

    QALLOC_NODISCARD QALLOC_CONSTEXPR_14
    iterator end() noexcept {
        return iterator(m_last);
    }
private:
    pointer   m_data;
    pointer   m_last;
    size_type m_capacity;
    size_type m_size;

    static constexpr size_type INITIAL_SIZE = 8_z;

    QALLOC_MALLOC_FUNCTION(
    inline static pointer allocate_n_elements(size_type n)) {
        return static_cast<pointer>(q_malloc(sizeof(value_type) * n));
    }

    template <typename Iterator> QALLOC_CONSTEXPR_14
    static void move(Iterator src, Iterator dst) {
        new (dst.base()) typename Iterator::value_type(std::move(*src));
    }
};

QALLOC_INTERNAL_END
#endif