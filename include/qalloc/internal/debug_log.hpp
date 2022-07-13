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

/// @file qalloc/internal/debug_log.hpp
/// @brief qalloc debug log header file.
/// @author yusing
/// @date 2022-07-02

#ifndef QALLOC_DEBUG_LOG_HPP
#define QALLOC_DEBUG_LOG_HPP

#include <cstdio>  // std::printf
#include <cstddef> // std::size_t
#include <atomic>  // std::atomic
#include <qalloc/internal/defs.hpp>

#if QALLOC_DEBUG
QALLOC_BEGIN
static FILE* log_file = nullptr;

template <typename ...Args>
inline void debug_log(const char* format, Args&&... args) {
    QALLOC_PRINTF(format, args...);
    if (log_file == nullptr) {
        QALLOC_FOPEN(&log_file, "qalloc.log", "w");
    }
    if (log_file == nullptr) {
        return;
    }
    QALLOC_FPRINTF(log_file, format, std::forward<Args>(args)...);
}

QALLOC_END
#else
    #define debug_log(...) (void)0
#endif // QALLOC_DEBUG

#endif // QALLOC_DEBUG_LOG_HPP