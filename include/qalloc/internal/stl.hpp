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

/// @file qalloc/internal/std_types.hpp
/// @brief qalloc STL container types header file.
/// @author yusing
/// @date 2022-07-02

#ifndef QALLOC_STL_HPP
#define QALLOC_STL_HPP

#include <list>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <qalloc/internal/allocator.hpp>
#include <qalloc/internal/defs.hpp>

QALLOC_BEGIN

/// @brief STL container types with type info and gc support.
namespace stl {

template <typename T, typename TAllocator = qalloc::allocator<T>>
using vector = std::vector<T, TAllocator>;

template <typename TKey, typename TValue, typename TLess = std::less<TKey>, typename TAllocator = qalloc::allocator<std::pair<const TKey, TValue>>>
using map = std::map<TKey, TValue, TLess, TAllocator>;

template <typename TKey, typename TValue, typename THash = std::hash<TKey>, typename TEqualTo = std::equal_to<TKey>, typename TAllocator = qalloc::allocator<std::pair<const TKey, TValue>>>
using unordered_map = std::unordered_map<TKey, TValue, THash, TEqualTo, TAllocator>;

template <typename T, typename TLess = std::less<T>, typename TAllocator = qalloc::allocator<T>>
using set = std::set<T, TLess, TAllocator>;

template <typename T, typename THash = std::hash<T>, typename TEqualTo = std::equal_to<T>, typename TAllocator = qalloc::allocator<T>>
using unordered_set = std::unordered_set<T, THash, TEqualTo, TAllocator>;

template <typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAllocator = qalloc::allocator<TChar>>
using basic_string = std::basic_string<TChar, TCharTraits, TAllocator>;
using string = qalloc::stl::basic_string<char>;

template <typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAllocator = qalloc::allocator<TChar>>
using basic_stringstream = std::basic_stringstream<TChar, TCharTraits, TAllocator>;
using stringstream = qalloc::stl::basic_stringstream<char>;

template <typename T, typename TAllocator = qalloc::allocator<T>>
using list = std::list<T, TAllocator>;

template <typename T, typename TAllocator = qalloc::allocator<T>>
using deque = std::deque<T, TAllocator>;

}

/// @brief STL container types with no type info and gc support.
namespace simple {

template <typename T, typename TAllocator = qalloc::simple_allocator<T>>
using vector = std::vector<T, TAllocator>;

template <typename TKey, typename TValue, typename TLess = std::less<TKey>, typename TAllocator = qalloc::simple_allocator<std::pair<const TKey, TValue>>>
using map = std::map<TKey, TValue, TLess, TAllocator>;

template <typename TKey, typename TValue, typename THash = std::hash<TKey>, typename TEqualTo = std::equal_to<TKey>, typename TAllocator = qalloc::simple_allocator<std::pair<const TKey, TValue>>>
using unordered_map = std::unordered_map<TKey, TValue, THash, TEqualTo, TAllocator>;

template <typename T, typename TLess = std::less<T>, typename TAllocator = qalloc::simple_allocator<T>>
using set = std::set<T, TLess, TAllocator>;

template <typename T, typename THash = std::hash<T>, typename TEqualTo = std::equal_to<T>, typename TAllocator = qalloc::simple_allocator<T>>
using unordered_set = std::unordered_set<T, THash, TEqualTo, TAllocator>;

template <typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAllocator = qalloc::simple_allocator<TChar>>
using basic_string = std::basic_string<TChar, TCharTraits, TAllocator>;
using string = qalloc::simple::basic_string<char>;

template <typename TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAllocator = qalloc::simple_allocator<TChar>>
using basic_stringstream = std::basic_stringstream<TChar, TCharTraits, TAllocator>;
using stringstream = qalloc::simple::basic_stringstream<char>;

template <typename T, typename TAllocator = qalloc::simple_allocator<T>>
using list = std::list<T, TAllocator>;

template <typename T, typename TAllocator = qalloc::simple_allocator<T>>
using deque = std::deque<T, TAllocator>;

} // namespace simple

using namespace stl;

QALLOC_END
#endif // QALLOC_STL_HPP