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

/// @file qalloc/internal/optional.hpp
/// @brief qalloc optional class header file.
/// @author yusing
/// @date 2022-07-13

#ifndef QALLOC_OPTIONAL_HPP
#define QALLOC_OPTIONAL_HPP

#include <qalloc/internal/defs.hpp>

QALLOC_BEGIN

enum class opt_null_t : bool { Value };
static constexpr opt_null_t QALLOC_OPT_NULL = opt_null_t::Value;

/// @brief Simple std::optional like class.
template <typename T>
class optional {
public:
    optional() noexcept : m_has_value(false) {}
    optional(const optional&) noexcept = default;
    optional(optional&&) noexcept = default;
    optional& operator=(const optional&) noexcept = default;
    optional& operator=(optional&&) noexcept = default;
    optional& operator=(opt_null_t) noexcept {
        m_has_value = false;
        return *this;
    }
    optional& operator=(const T& value) noexcept {
        m_has_value = true;
        m_value = value;
        return *this;
    }
    operator T&() noexcept { return m_value; } // NOLINT(google-explicit-constructor)
    constexpr bool has_value() {
        return m_has_value;
    }
private:
    bool m_has_value = false;
    T    m_value;
};

QALLOC_END
#endif //QALLOC_OPTIONAL_HPP
