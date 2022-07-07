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

/// @file qalloc/internal/experimental/list.hpp
/// @brief qalloc experimental list class header file.
/// @author yusing
/// @date 2022-07-05

#ifndef QALLOC_LIST_HPP
#define QALLOC_LIST_HPP

#include <cstddef>
#include <memory>
#include <cassert>
#include <qalloc/internal/experimental/iterator.hpp>
#include <qalloc/internal/memory.hpp>

QALLOC_INTERNAL_BEGIN

template <class T>
class list {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer   QALLOC_MAYBE_UNUSED = const T*;
    using const_reference QALLOC_MAYBE_UNUSED = const T&;
    using reference = T&;

    struct node_type : public T {
        // don't use value_type here
        // because when T::value_type exists
        // it maybe used instead of T
        node_type* next;
        explicit node_type(T&& lv) : T(std::forward<T>(lv)),
                                              next(nullptr) {}
        static node_type* set(void_pointer p_this, T&& lv) {
            return new (p_this) node_type(std::forward<T>(lv));
        }
    };
    using node_pointer = node_type*;

    using iterator = class list_iterator : public qalloc::internal::iterator<node_type> {
    public:
      using iterator_category QALLOC_MAYBE_UNUSED = std::forward_iterator_tag;
      using qalloc::internal::iterator<node_type>::iterator;
      list_iterator& operator++() {
        assert(this->m_p != nullptr);
        this->m_p = this->m_p->next;
        return *this;
      }
      list_iterator operator++(int) {
        assert(this->m_p != nullptr);
        list_iterator it = *this;
        ++(*this);
        return it;
      }
    };

    list() = delete;
    list(const list&) = delete;
    list(list&&) = delete;
    list& operator=(const list&) = delete;
    list& operator=(list&&) = delete;
    inline list(size_type, value_type&& first) : m_size(1) {
        void_pointer ptr  = q_malloc(sizeof(node_type));
        node_pointer node = node_type::set(ptr, std::forward<value_type>(first));
        m_head = node;
        m_tail = node;
    }
    // no need to support copy constructor and move constructor
    // because we don't use them
    // it is only used for pool
    // once initialized by pool, it will never be copied nor moved
    inline ~list() {
        while (m_head) {
            auto* next = m_head->next;
            m_head->~node_type();
            q_free(m_head);
            m_head = next;
        }
    }
    template <typename ...Args>
    void emplace_back(Args&& ...args) {
        void_pointer ptr  = q_malloc(sizeof(node_type));
        node_pointer node = node_type::set(ptr, std::forward<Args>(args)...);
        m_tail->next = node;
        m_tail = node;
        ++m_size;
    }
    QALLOC_NODISCARD inline reference front() const {
        assert(m_head != nullptr);
        return *m_head;
    }
    QALLOC_NODISCARD inline reference back() const {
        assert(m_tail != nullptr);
        return *m_tail;
    }
    QALLOC_NODISCARD constexpr bool empty() const noexcept {
        return m_head == nullptr;
    }
    QALLOC_NODISCARD constexpr size_type size() const noexcept {
        return m_size;
    }

    inline iterator begin() {
        return iterator(m_head);
    }

    inline iterator end() {
        return iterator(nullptr);
    }
private:
    node_pointer m_head;
    node_pointer m_tail;
    size_type m_size;
};

QALLOC_INTERNAL_END
#endif