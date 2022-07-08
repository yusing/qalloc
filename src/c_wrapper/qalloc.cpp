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

/// @file qalloc.cpp
/// @brief qalloc c wrapper library implementation file.
/// @author yusing
/// @date 2022-07-07

#define QALLOC_C_GLOBAL_POOL (qalloc::internal::get_pool<int>())
#define _Py_IS_ALIGNED(p, a) (!((uintptr_t)(p) & (uintptr_t)((a) - 1)))

#include <utility>
#include <cstring>
#include <qalloc/qalloc.h>
#include <qalloc/internal/pool.hpp>
#include <qalloc/internal/pool_impl.hpp>
#include <qalloc/internal/pool_base_impl.hpp>
#include <qalloc/internal/global_pool.hpp>
#include <qalloc/internal/pointer.hpp>

extern "C" {
using std::size_t;
using std::uintptr_t;
using std::uint8_t;
using qalloc::byte;
using qalloc::pointer::sub;
using qalloc::operator ""_z;

/// @brief allocate memory aligned to 4 bytes
/// @details allocated in following structure <br>
/// | -------------- Structure -------------- |<br>
/// |   size | padding | padding length | @b data |<br>
/// | --------------------------------------- |
/// @param size the size of the memory to allocate.
/// @return pointer to the @b data.
QALLOC_EXPORT void* q_allocate(size_t size) {
    constexpr size_t SIZE_LONG = sizeof(long);
    size_t allocated_size = size + sizeof(size_t) + SIZE_LONG + sizeof(uint8_t);
    byte* p = QALLOC_C_GLOBAL_POOL->detailed_allocate<void>(allocated_size );
    byte* origin = p;
    if (p == nullptr) {
        return nullptr;
    }
    // size start
    new (p) size_t(size);
    p += sizeof(size_t);
    // padding start
    uint8_t padding = 0;
    while((uintptr_t)(p + padding + sizeof(uint8_t)) % SIZE_LONG != 0) {
        padding++;
    }
    p += padding;
    new (p) uint8_t(padding);
    p += sizeof(uint8_t);
    QALLOC_ASSERT(_Py_IS_ALIGNED(p, SIZE_LONG));
    QALLOC_ASSERT(p + size < origin + allocated_size);
    return p;
}

QALLOC_EXPORT void* q_callocate(size_t n, size_t size) {
    if ((n | size) == 0_z) {
        return nullptr;
    }
    n *= size;
    void* addr = q_allocate(n);
    if (addr == nullptr) {
        return nullptr;
    }
    return std::memset(addr, 0, n);
}

QALLOC_EXPORT void q_deallocate(void* ptr) {
    if (ptr == nullptr) {
        return;
    }
    uint8_t& padding_len = *sub<uint8_t*>(ptr, sizeof(uint8_t));
    size_t&  data_size   = *sub<size_t*>(&padding_len, padding_len + sizeof(size_t));
    size_t   before_data = sizeof(size_t) + padding_len + sizeof(uint8_t);
    QALLOC_C_GLOBAL_POOL->detailed_deallocate<void>(
            reinterpret_cast<byte *>(&data_size),
        before_data + data_size
    );
}

QALLOC_EXPORT void* q_reallocate(void* ptr, size_t new_size) {
    if (ptr == nullptr) {
        return q_allocate(new_size);
    }
    uint8_t& padding_len = *sub<uint8_t*>(ptr, sizeof(uint8_t));
    size_t&  old_size    = *sub<size_t*>(&padding_len, padding_len + sizeof(size_t));
    size_t   before_data = sizeof(size_t) + padding_len + sizeof(uint8_t);
    void*    new_data    = std::memcpy(q_allocate(new_size), ptr, std::min(old_size, new_size));
    QALLOC_C_GLOBAL_POOL->detailed_deallocate<void>(
            reinterpret_cast<byte *>(&old_size),
            before_data + old_size
    );
    return new_data;
}

QALLOC_EXPORT size_t q_garbage_collect() {
    return QALLOC_C_GLOBAL_POOL->gc();
}

}