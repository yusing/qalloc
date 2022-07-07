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

/// @file qalloc/internal/pool.hpp
/// @brief qalloc pool class header file.
/// @author yusing
/// @date 2022-07-06

#ifndef QALLOC_POOL_HPP
#define QALLOC_POOL_HPP

#include <qalloc/internal/pool_base.hpp>
#include <qalloc/internal/defs.hpp>

QALLOC_BEGIN

/// @brief qalloc pool class.
class pool_t : public pool_base_t {
public:
    using pool_base_t::pool_base_t;
    using pool_base_t::operator new;
    using pool_base_t::operator delete;
    template <class T>
    byte_pointer detailed_allocate(size_type n_bytes_requested) const;
    template <class T>
    void detailed_deallocate(byte_pointer p, size_type n_bytes_requested) const;
    size_type gc() const;
}; // class pool_t

using pool_pointer = const pool_t*;

QALLOC_END
#endif //QALLOC_POOL_HPP
