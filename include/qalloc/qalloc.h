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

/// @file qalloc/qalloc.h
/// @brief qalloc c wrapper library header file.
/// @author yusing
/// @date 2022-07-07

#ifndef QALLOC_QALLOC_H
#define QALLOC_QALLOC_H

/// @brief <qalloc_export.h> is CMake generated header file.
#if __has_include(<qalloc_export.h>)
    #include <qalloc_export.h>
#else
    #define QALLOC_EXPORT
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif // __cplusplus

/// @brief allocates memory from the global pool.
/// @param size the size of the memory to allocate.
/// @return a pointer to the allocated memory.
QALLOC_EXPORT void* q_allocate(size_t size);

/// @brief allocates memory from the global pool.
/// @param size the size of the memory to allocate.
/// @return a pointer to the allocated memory.
QALLOC_EXPORT void* q_callocate(size_t n, size_t size);

/// @brief deallocates memory from the global pool.
/// @param ptr the pointer to the memory to deallocate.
/// @return void.
QALLOC_EXPORT void q_deallocate(void* ptr);

/// @brief reallocate memory from the global pool.
/// @param ptr the pointer to the memory to reallocate.
/// @param size the size of the memory to reallocate.
/// @return a pointer to the reallocated memory.
QALLOC_EXPORT void* q_reallocate(void* ptr, size_t new_size);

/// @brief garbage collects the global pool.
/// @return the number of bytes freed.
QALLOC_EXPORT size_t q_garbage_collect();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //QALLOC_QALLOC_H
