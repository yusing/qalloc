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

QALLOC_BEGIN
#ifdef QALLOC_WINDOWS
    void_pointer q_malloc(size_type n_bytes) {
        return VirtualAlloc(nullptr, n_bytes, MEM_COMMIT, PAGE_READWRITE);
    }
    void_pointer q_realloc(void_pointer p, size_type n_bytes) {
        return VirtualAlloc(p, n_bytes, MEM_COMMIT, PAGE_READWRITE);
    }
    void q_free(void_pointer p) {
        VirtualFree(p, 0, MEM_RELEASE);
    }
#else // QALLOC_WINDOWS
    #define q_free std::free
    void_pointer q_malloc(size_type n_bytes) {
        QALLOC_RESTRICT void_pointer p = std::malloc(n_bytes);
        if (p == nullptr) {
            throw std::bad_alloc();
        }
        return p;
    }
    void_pointer q_realloc(void_pointer p, size_type n_bytes) {
        QALLOC_RESTRICT void_pointer q = std::realloc(p, n_bytes);
        if (q == nullptr) {
            throw std::bad_alloc();
        }
        return q;
    }
#endif // QALLOC_WINDOWS
QALLOC_END
#endif //QALLOC_MEMORY_HPP