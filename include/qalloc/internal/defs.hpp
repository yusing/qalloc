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

/// @file qalloc/internal/defs.hpp
/// @brief qalloc macro definitions header file.
/// @author yusing
/// @date 2022-07-02

#ifndef QALLOC_DEFS_HPP
#define QALLOC_DEFS_HPP

#include <ciso646>
#include <version>
#include <cstddef>

#if !defined(__cplusplus) || __cplusplus == 199711L
    #ifdef _MSVC_LANG
        #define QALLOC_CPP_STANDARD _MSVC_LANG
    #else
        #error "C++ standard not defined"
    #endif // _MSVC_LANG
#else // !defined(__cplusplus) || __cplusplus == 199711L
    #define QALLOC_CPP_STANDARD __cplusplus
#endif // __cplusplus

#ifdef __has_cpp_attribute
#   define QALLOC_HAS_CPP_ATTRIBUTE(NAME) __has_cpp_attribute(NAME) || QALLOC_CPP_STANDARD >= 201703L
#else
#   define QALLOC_HAS_CPP_ATTRIBUTE(NAME) __cplusplus >= 201703L
#endif // __has_cpp_attribute

#if QALLOC_HAS_CPP_ATTRIBUTE(maybe_unused)
#   define QALLOC_MAYBE_UNUSED [[maybe_unused]]
#else
#   define QALLOC_MAYBE_UNUSED
#endif // QALLOC_HAS_CPP_ATTRIBUTE(maybe_unused)

#if QALLOC_HAS_CPP_ATTRIBUTE(nodiscard)
#   define QALLOC_NODISCARD [[nodiscard]]
#else
#   define QALLOC_NODISCARD
#endif // QALLOC_HAS_CPP_ATTRIBUTE(nodiscard)

#if QALLOC_CPP_STANDARD >= 201400L
    #define QALLOC_CONSTEXPR_14 constexpr
    #define QALLOC_CONSTEXPR_14_C static constexpr
    #define QALLOC_CXX_14 1
    #define QALLOC_STRINGVIEW std::string_view
#else
    #define QALLOC_CONSTEXPR_14 inline
    #define QALLOC_CONSTEXPR_14_C static const
    #define QALLOC_CXX_14 0
    #define QALLOC_STRINGVIEW qalloc::string
#endif // QALLOC_CPP_STANDARD >= 201400L

#if QALLOC_CPP_STANDARD >= 201703L
    #define QALLOC_IF_CONSTEXPR if constexpr
    #define QALLOC_CXX_17 1
#else
    #define QALLOC_IF_CONSTEXPR if
    #define QALLOC_CXX_17 0
#endif // QALLOC_CPP_STANDARD >= 201703L

#if defined(_WIN32) || defined(_WIN64)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif // NOMINMAX
    #include <Windows.h>
#endif // defined(_WIN32) || defined(_WIN64)

#ifdef _MSVC_LANG
    #define QALLOC_RESTRICT __restrict
    #define QALLOC_MALLOC_FUNCTION(DECLARATION) QALLOC_NODISCARD __declspec(restrict) __declspec(noalias) DECLARATION
    #define QALLOC_FOPEN(PTR, FILENAME, MODE) fopen_s((PTR), (FILENAME), (MODE))
#else // _MSVC_LANG
    #define QALLOC_RESTRICT __restrict__
    #define QALLOC_MALLOC_FUNCTION(DECLARATION) QALLOC_NODISCARD DECLARATION
    #define QALLOC_FOPEN(PTR, FILENAME, MODE) (*PTR) = fopen((FILENAME), (MODE))
#endif // _MSVC_LANG

#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    #include <cassert>
    #define QALLOC_DEBUG 1
    #define QALLOC_DEBUG_STATEMENT(STMT) STMT
    #define QALLOC_NDEBUG_CONSTEXPR inline
        #define QALLOC_ASSERT(expr) assert(expr)
    #ifndef QALLOC_STORE_TYPEINFO
        #define QALLOC_STORE_TYPEINFO 1
    #endif // QALLOC_STORE_TYPEINFO
#else // !defined(NDEBUG)
    #define QALLOC_DEBUG 0
    #define QALLOC_DEBUG_STATEMENT(...)
    #define QALLOC_NDEBUG_CONSTEXPR constexpr
    #define QALLOC_ASSERT(expr) (void)0
    #ifndef QALLOC_STORE_TYPEINFO
        #define QALLOC_STORE_TYPEINFO 0
    #endif // QALLOC_STORE_TYPEINFO
#endif // !defined(NDEBUG)

#if defined(__has_include) && __has_include(<cxxabi.h>)
    #include <cxxabi.h>
    #define QALLOC_CXA_DEMANGLE 1
#else // !(defined(__has_include) && __has_include(<cxxabi.h>))
    #define QALLOC_CXA_DEMANGLE 0
#endif // defined(__has_include) && __has_include(<cxxabi.h>)

#define QALLOC_BEGIN namespace qalloc {
#define QALLOC_END }
#define QALLOC_INTERNAL_BEGIN QALLOC_BEGIN namespace internal {
#define QALLOC_INTERNAL_END } QALLOC_END
#define QALLOC_PRINTF (void) std::printf
#define QALLOC_FPRINTF (void) std::fprintf

#endif // QALLOC_DEFS_HPP