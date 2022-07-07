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

/// @file qalloc/internal/subpool.hpp
/// @brief qalloc sub pool class header file.
/// @author yusing
/// @date 2022-07-04

#ifndef QALLOC_SUBPOOL_HPP
#define QALLOC_SUBPOOL_HPP

#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/pointer.hpp>

QALLOC_BEGIN
/// @brief qalloc subpool class.
struct subpool_t { // use byte pointer for easier pointer arithmetics
    const_byte_pointer  begin;
    const_byte_pointer  end;
    byte_pointer        pos;
    size_type           size;
}; // struct subpool_t
QALLOC_END

#endif //QALLOC_SUBPOOL_HPP
