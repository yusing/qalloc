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

/* Sample output:
[pool] pool of 256 bytes constructed
[allocate] allocated 32 bytes @ 0000027F7A57BBE0 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 0000027F7A57BC00 (Thread 0 Subpool 1)
[allocate] allocated 96 bytes @ 0000027F7A57BC38 (Thread 0 Subpool 1)
[allocate] adding new subpool with size 512 (Thread 0 Subpool 1)
[allocate] subpool 1 has 72 bytes left @ 0000027F7A57BC98 (Thread 0)
[deallocate] deallocated 72 bytes @ 0000027F7A57BC98 (Thread 0 Subpool 1)
[allocate] allocated 136 bytes @ 0000027F7A587600 (Thread 0 Subpool 2)
[deallocate] merged 0000027F7A57BC38 (96 bytes) and 0000027F7A57BC98 (72 bytes) into 168 bytes (Thread 0)
[deallocate] merged 0000027F7A57BC00 (56 bytes) and 0000027F7A57BC38 (168 bytes) into 224 bytes (Thread 0)
[allocate] reused freed block of 224 bytes and has 48 bytes left @ 0000027F7A57BC30 (Thread 0 Subpool 2)
[deallocate] deallocated 136 bytes @ 0000027F7A587600 (Thread 0 Subpool 2)
[allocate] allocated 256 bytes @ 0000027F7A587688 (Thread 0 Subpool 2)
[deallocate] deallocated 176 bytes @ 0000027F7A57BC30 (Thread 0 Subpool 2)
[allocate] adding new subpool with size 1024 (Thread 0 Subpool 2)
[allocate] subpool 2 has 120 bytes left @ 0000027F7A587788 (Thread 0)
[deallocate] deallocated 120 bytes @ 0000027F7A587788 (Thread 0 Subpool 2)
[allocate] allocated 376 bytes @ 0000027F7A587840 (Thread 0 Subpool 3)
[deallocate] merged 0000027F7A587688 (256 bytes) and 0000027F7A587788 (120 bytes) into 376 bytes (Thread 0)
[deallocate] merged 0000027F7A57BC00 (48 bytes) and 0000027F7A57BC30 (176 bytes) into 224 bytes (Thread 0)
[deallocate] merged 0000027F7A587600 (136 bytes) and 0000027F7A587688 (376 bytes) into 512 bytes (Thread 0)
[allocate] allocated 536 bytes @ 0000027F7A5879B8 (Thread 0 Subpool 3)
[deallocate] deallocated 376 bytes @ 0000027F7A587840 (Thread 0 Subpool 3)
class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
[pool] pool of 128 bytes constructed
[allocate] allocated 48 bytes @ 0000027F7A528910 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 0000027F7A528940 (Thread 0 Subpool 1)
[allocate] adding new subpool with size 256 (Thread 0 Subpool 1)
[allocate] subpool 1 has 24 bytes left @ 0000027F7A528978 (Thread 0)
[allocate] allocated 48 bytes @ 0000027F7A57CC20 (Thread 0 Subpool 2)
[allocate] allocated 52 bytes @ 0000027F7A57CC50 (Thread 0 Subpool 2)
[allocate] reused freed block of 24 bytes and has 0 bytes left @ 0000027F7A528978 (Thread 0 Subpool 2)
[allocate] allocated 36 bytes @ 0000027F7A57CC84 (Thread 0 Subpool 2)
[allocate] allocated 44 bytes @ 0000027F7A57CCA8 (Thread 0 Subpool 2)
[allocate] allocated 48 bytes @ 0000027F7A57CCD4 (Thread 0 Subpool 2)
[allocate] adding new subpool with size 512 (Thread 0 Subpool 2)
[allocate] subpool 2 has 28 bytes left @ 0000027F7A57CD04 (Thread 0)
[allocate] allocated 48 bytes @ 0000027F7A589EF0 (Thread 0 Subpool 3)
[allocate] allocated 36 bytes @ 0000027F7A589F20 (Thread 0 Subpool 3)
[deallocate] deallocated 44 bytes @ 0000027F7A57CCA8 (Thread 0 Subpool 3)
[deallocate] merged 0000027F7A57CCD4 (48 bytes) and 0000027F7A57CD04 (28 bytes) into 76 bytes (Thread 0)
[deallocate] merged 0000027F7A57CCA8 (44 bytes) and 0000027F7A57CCD4 (76 bytes) into 120 bytes (Thread 0)
[deallocate] merged 0000027F7A57CC84 (36 bytes) and 0000027F7A57CCA8 (120 bytes) into 156 bytes (Thread 0)
[deallocate] deallocated 24 bytes @ 0000027F7A528978 (Thread 0 Subpool 3)
[deallocate] deallocated 36 bytes @ 0000027F7A589F20 (Thread 0 Subpool 3)
[deallocate] merged 0000027F7A589EF0 (48 bytes) and 0000027F7A589F20 (36 bytes) into 84 bytes (Thread 0)
[deallocate] merged 0000027F7A528940 (56 bytes) and 0000027F7A528978 (24 bytes) into 80 bytes (Thread 0)
[deallocate] merged 0000027F7A57CC50 (52 bytes) and 0000027F7A57CC84 (156 bytes) into 208 bytes (Thread 0)
[deallocate] merged 0000027F7A528910 (48 bytes) and 0000027F7A528940 (80 bytes) into 128 bytes (Thread 0)
[deallocate] merged 0000027F7A57CC20 (48 bytes) and 0000027F7A57CC50 (208 bytes) into 256 bytes (Thread 0)
[gc]: subpool 1 released (128 bytes)
[gc]: subpool 2 released (256 bytes)
mem_freed: 384 bytes
Memory Pool:  Usage: 0 of 512 bytes (0%)
  Subpools:
    1: released by gc
    2: released by gc
    3: 0000027F7A589EF0 ~ 0000027F7A58A0F0 (512 bytes)
      Position @ 0000027F7A589F44
      428 bytes unused

  Deallocated blocks:
    0000027F7A589EF0: 84 bytes
      Subpool: 3, type: int

[allocate] reused freed block of 84 bytes and has 68 bytes left @ 0000027F7A589F34 (Thread 0 Subpool 3)
Memory Pool:  Usage: 16 of 512 bytes (3%)
  Subpools:
    1: released by gc
    2: released by gc
    3: 0000027F7A589EF0 ~ 0000027F7A58A0F0 (512 bytes)
      Position @ 0000027F7A589F44
      428 bytes unused

  Deallocated blocks:
    0000027F7A589EF0: 68 bytes
      Subpool: 3, type: int

[pool] pool of 128 bytes constructed
[allocate] allocated 56 bytes @ 0000027F7A528910 (Thread 0 Subpool 1)
0, 1, 2, 3, 4,
Memory Pool:  Usage: 56 of 128 bytes (43%)
  Subpools:
    1: 0000027F7A528910 ~ 0000027F7A528990 (128 bytes)
      Position @ 0000027F7A528948
      72 bytes unused

  Deallocated blocks:

[deallocate] deallocated 536 bytes @ 0000027F7A5879B8 (Thread 0 Subpool 3)
[deallocate] merged 0000027F7A57BBE0 (32 bytes) and 0000027F7A57BC00 (224 bytes) into 256 bytes (Thread 0)
[deallocate] merged 0000027F7A587840 (376 bytes) and 0000027F7A5879B8 (536 bytes) into 912 bytes (Thread 0)
*/