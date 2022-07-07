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

/// @file example.cpp
/// @brief qalloc example file.
/// @author yusing
/// @date 2022-07-02

#define QALLOC_STORE_TYPEINFO 1
#include <qalloc/qalloc.hpp>
#include <random>

using namespace qalloc;

int main() {
    // Example 1: use qalloc stl container type
    qalloc::vector<std::string> v;
    for (int i = 0; i < 10; i++) {
        v.push_back("hi");
    }
    void_pointer p_v = v.data();
    std::cout << demangled_type_name_of(p_v) << std::endl; // std::basic_string<char, std::char_traits<char>, allocator<char>>

    // Example 2: use qalloc:pool_t
    constexpr size_type TEST_SIZE = 10_z;
    std::default_random_engine generator(1000_z); // NOLINT(cert-msc51-cpp)
    std::uniform_int_distribution<size_type> distribution(1_z,TEST_SIZE);

    auto* new_pool = new pool_t(128_z); // allocate a new pool of 128 bytes
    allocator<int> allocator(new_pool); // use the new pool as allocator

    std::vector<std::pair<int*, size_type>> allocated_pairs;

    for (size_type i = 0; i < TEST_SIZE; i++) { // emplace some elements
        size_type n_bytes = distribution(generator);
        (void)allocated_pairs.emplace_back(allocator.allocate(n_bytes), n_bytes);
    }
    for (size_type i = 0; i < TEST_SIZE; i++) { // deallocate all elements
        size_type idx = distribution(generator) % allocated_pairs.size();
        auto& pair = allocated_pairs[idx];
        allocator.deallocate(pair.first, pair.second);
        allocated_pairs.erase(allocated_pairs.begin() + idx);
    }
    size_type mem_freed = new_pool->gc(); // garbage collect the pool
    std::cout << "mem_freed: " << mem_freed << " bytes" << std::endl;
    new_pool->print_info(); // print pool info
    (void)new_pool->allocate(16_z); // allocate 16 bytes from gc-ed pool
    new_pool->print_info(); // print pool info

    // Example 3: use qalloc::allocator<T> with qalloc::pool_t
    auto* d_pool = new pool_t(128_z); // allocate a new pool of 128 bytes
    qalloc::allocator<double> d_allocator(d_pool); // use the new pool as allocator
    auto* d_array = d_allocator.allocate(5_z); // allocate 5 double elements
    for (int i = 0; i < 5; i++) {
        d_array[i] = static_cast<double>(i); // initialize the elements
    }
    for (int i = 0; i < 5; i++) {
        std::cout << d_array[i] << ", "; // print the elements
    }
    std::cout << std::endl;
    d_pool->print_info(); // print pool info
    return 0;
}