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

/// @file qalloc/internal/memory.hpp
/// @brief qalloc memory utilities header file.
/// @author yusing
/// @date 2022-07-04

#ifndef QALLOC_MEMORY_HPP
#define QALLOC_MEMORY_HPP

#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/pointer.hpp>

#define q_free(PTR) std::free(PTR)

QALLOC_BEGIN
void_pointer q_malloc(size_type n_bytes) {
    QALLOC_RESTRICT void_pointer p = std::malloc(n_bytes);
    if (p == nullptr) {
        throw std::bad_alloc();
    }
    return p;
}
QALLOC_END
#endif