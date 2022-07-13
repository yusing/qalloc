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

/// @file qalloc/internal/type_info.hpp
/// @brief qalloc type info header file.
/// @author yusing
/// @date 2022-07-02

#ifndef QALLOC_TYPE_INFO_HPP
#define QALLOC_TYPE_INFO_HPP

#include <typeinfo>
#include <unordered_map>
#include <cstring> // std::strlen, std::memcpy
#include <qalloc/internal/defs.hpp>
#include <qalloc/internal/pointer.hpp>
#include <qalloc/internal/block.hpp>
#include <qalloc/internal/stl.hpp>

QALLOC_BEGIN
/// @brief get type info of pointer.
/// @param p pointer allocated from qalloc pool.
/// @return type info of pointer.
constexpr const std::type_info& type_of(void_ptr_t p) {
    return *block_info_t::of(p)->type_info;
}

/// @brief get raw type name of object in pointer.
/// @param p pointer allocated from qalloc pool.
/// @return @b c-string of raw type name of object in pointer.
inline const char* type_name_of(void_ptr_t p) {
    return type_of(p).name();
}

/// @brief get demangled type name from mangled type name.
/// @param mangled_name mangled type name.
/// @return @b std::string of demangled type name.
std::string demangled_type_name_of(const char* mangled_name) {
#if QALLOC_CXA_DEMANGLE
    int status;
    char* demangled_name = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
    if (status != 0) {
        throw std::runtime_error("Cannot demangle type name");
    }
    std::string result(demangled_name);
    std::free(demangled_name);
    return result;
#else
    return {mangled_name};
#endif
}

/// @brief get demangled type name of object in pointer.
/// @param p pointer allocated from qalloc pool.
/// @return @b std::string of demangled type name of object in pointer.
std::string demangled_type_name_of(void_ptr_t p) {
    return demangled_type_name_of(type_name_of(p));
}

/// @brief cast pointer to another type with type check.
/// @tparam T type to cast to.
/// @param p pointer allocated from qalloc pool.
/// @return a reference to pointer of type @b T.
template <typename T> QALLOC_MAYBE_UNUSED
T& safe_cast(void_ptr_t p) {
    if(type_of(p) != typeid(T)) {
        throw std::bad_cast();
    }
    return *static_cast<T*>(p);
}
QALLOC_END

#endif // QALLOC_TYPE_INFO_HPP