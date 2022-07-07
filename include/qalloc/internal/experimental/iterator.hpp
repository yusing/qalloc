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

/// @file qalloc/internal/experimental/iterator.hpp
/// @brief qalloc experimental iterator class header file.
/// @author yusing
/// @date 2022-07-05

#ifndef QALLOC_ITERATOR_HPP
#define QALLOC_ITERATOR_HPP

#include "qalloc/internal/defs.hpp"
QALLOC_INTERNAL_BEGIN

template <typename T>
class iterator {
public:
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using iterator_category = std::input_iterator_tag;

    QALLOC_CONSTEXPR_14
    explicit iterator(pointer p) : m_p(p) {}

    QALLOC_CONSTEXPR_14
    reference operator*() const {
        return *m_p;
    }

    QALLOC_CONSTEXPR_14
    pointer operator->() const {
        return m_p;
    }

    QALLOC_CONSTEXPR_14
    bool operator==(const iterator& rhs) const {
        return m_p == rhs.m_p;
    }

    QALLOC_CONSTEXPR_14
    bool operator!=(const iterator& rhs) const {
        return m_p != rhs.m_p;
    }

    QALLOC_CONSTEXPR_14
    bool operator<(const iterator& rhs) const {
        return m_p < rhs.m_p;
    }

    QALLOC_CONSTEXPR_14
    bool operator>(const iterator& rhs) const {
        return m_p > rhs.m_p;
    }

    QALLOC_CONSTEXPR_14
    bool operator<=(const iterator& rhs) const {
        return m_p <= rhs.m_p;
    }

    QALLOC_CONSTEXPR_14
    bool operator>=(const iterator& rhs) const {
        return m_p >= rhs.m_p;
    }
protected:
    pointer m_p;
}; // class iterator

QALLOC_INTERNAL_END
#endif // QALLOC_ITERATOR_HPP