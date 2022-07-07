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
[allocate] allocated 32 bytes @ 00000161EA961F30 (Thread 0 Subpool 1)
[allocate] allocated 32 bytes @ 00000161EA961F30 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 00000161EA961F50 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 00000161EA961F50 (Thread 0 Subpool 1)
[allocate] allocated 96 bytes @ 00000161EA961F88 (Thread 0 Subpool 1)
[allocate] allocated 96 bytes @ 00000161EA961F88 (Thread 0 Subpool 1)
[allocate] adding new subpool with size 512 (Thread 0 Subpool 1)
[allocate] adding new subpool with size 512 (Thread 0 Subpool 1)
[allocate] subpool 1 has 72 bytes left @ 00000161EA961FE8 (Thread 0)
[allocate] subpool 1 has 72 bytes left @ 00000161EA961FE8 (Thread 0)
[deallocate] deallocated 72 bytes @ 00000161EA961FE8 (Thread 0 Subpool 1)
[deallocate] deallocated 72 bytes @ 00000161EA961FE8 (Thread 0 Subpool 1)
[allocate] allocated 136 bytes @ 00000161EA957D90 (Thread 0 Subpool 2)
[allocate] allocated 136 bytes @ 00000161EA957D90 (Thread 0 Subpool 2)
[deallocate] merged 00000161EA961F88 (96 bytes) and 00000161EA961FE8 (72 bytes) into 168 bytes (Thread 0)
[deallocate] merged 00000161EA961F88 (96 bytes) and 00000161EA961FE8 (72 bytes) into 168 bytes (Thread 0)
[deallocate] merged 00000161EA961F50 (56 bytes) and 00000161EA961F88 (168 bytes) into 224 bytes (Thread 0)
[deallocate] merged 00000161EA961F50 (56 bytes) and 00000161EA961F88 (168 bytes) into 224 bytes (Thread 0)
[allocate] reused freed block of 224 bytes and has 48 bytes left @ 00000161EA961F80 (Thread 0 Subpool 2)
[allocate] reused freed block of 224 bytes and has 48 bytes left @ 00000161EA961F80 (Thread 0 Subpool 2)
[deallocate] deallocated 136 bytes @ 00000161EA957D90 (Thread 0 Subpool 2)
[deallocate] deallocated 136 bytes @ 00000161EA957D90 (Thread 0 Subpool 2)
[allocate] allocated 256 bytes @ 00000161EA957E18 (Thread 0 Subpool 2)
[allocate] allocated 256 bytes @ 00000161EA957E18 (Thread 0 Subpool 2)
[deallocate] deallocated 176 bytes @ 00000161EA961F80 (Thread 0 Subpool 2)
[deallocate] deallocated 176 bytes @ 00000161EA961F80 (Thread 0 Subpool 2)
[allocate] adding new subpool with size 1024 (Thread 0 Subpool 2)
[allocate] adding new subpool with size 1024 (Thread 0 Subpool 2)
[allocate] subpool 2 has 120 bytes left @ 00000161EA957F18 (Thread 0)
[allocate] subpool 2 has 120 bytes left @ 00000161EA957F18 (Thread 0)
[deallocate] deallocated 120 bytes @ 00000161EA957F18 (Thread 0 Subpool 2)
[deallocate] deallocated 120 bytes @ 00000161EA957F18 (Thread 0 Subpool 2)
[allocate] allocated 376 bytes @ 00000161EA976CD0 (Thread 0 Subpool 3)
[allocate] allocated 376 bytes @ 00000161EA976CD0 (Thread 0 Subpool 3)
[deallocate] merged 00000161EA957E18 (256 bytes) and 00000161EA957F18 (120 bytes) into 376 bytes (Thread 0)
[deallocate] merged 00000161EA957E18 (256 bytes) and 00000161EA957F18 (120 bytes) into 376 bytes (Thread 0)
[deallocate] merged 00000161EA957D90 (136 bytes) and 00000161EA957E18 (376 bytes) into 512 bytes (Thread 0)
[deallocate] merged 00000161EA957D90 (136 bytes) and 00000161EA957E18 (376 bytes) into 512 bytes (Thread 0)
[deallocate] merged 00000161EA961F50 (48 bytes) and 00000161EA961F80 (176 bytes) into 224 bytes (Thread 0)
[deallocate] merged 00000161EA961F50 (48 bytes) and 00000161EA961F80 (176 bytes) into 224 bytes (Thread 0)
[allocate] allocated 536 bytes @ 00000161EA976E48 (Thread 0 Subpool 3)
[allocate] allocated 536 bytes @ 00000161EA976E48 (Thread 0 Subpool 3)
[deallocate] deallocated 376 bytes @ 00000161EA976CD0 (Thread 0 Subpool 3)
[deallocate] deallocated 376 bytes @ 00000161EA976CD0 (Thread 0 Subpool 3)
class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
[pool] pool of 128 bytes constructed
[pool] pool of 128 bytes constructed
[allocate] allocated 48 bytes @ 00000161EA904CA0 (Thread 0 Subpool 1)
[allocate] allocated 48 bytes @ 00000161EA904CA0 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 00000161EA904CD0 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 00000161EA904CD0 (Thread 0 Subpool 1)
[allocate] adding new subpool with size 256 (Thread 0 Subpool 1)
[allocate] adding new subpool with size 256 (Thread 0 Subpool 1)
[allocate] subpool 1 has 24 bytes left @ 00000161EA904D08 (Thread 0)
[allocate] subpool 1 has 24 bytes left @ 00000161EA904D08 (Thread 0)
[allocate] allocated 48 bytes @ 00000161EA962070 (Thread 0 Subpool 2)
[allocate] allocated 48 bytes @ 00000161EA962070 (Thread 0 Subpool 2)
[allocate] allocated 52 bytes @ 00000161EA9620A0 (Thread 0 Subpool 2)
[allocate] allocated 52 bytes @ 00000161EA9620A0 (Thread 0 Subpool 2)
[allocate] reused freed block of 24 bytes and has 0 bytes left @ 00000161EA904D08 (Thread 0 Subpool 2)
[allocate] reused freed block of 24 bytes and has 0 bytes left @ 00000161EA904D08 (Thread 0 Subpool 2)
[allocate] allocated 36 bytes @ 00000161EA9620D4 (Thread 0 Subpool 2)
[allocate] allocated 36 bytes @ 00000161EA9620D4 (Thread 0 Subpool 2)
[allocate] allocated 44 bytes @ 00000161EA9620F8 (Thread 0 Subpool 2)
[allocate] allocated 44 bytes @ 00000161EA9620F8 (Thread 0 Subpool 2)
[allocate] allocated 48 bytes @ 00000161EA962124 (Thread 0 Subpool 2)
[allocate] allocated 48 bytes @ 00000161EA962124 (Thread 0 Subpool 2)
[allocate] adding new subpool with size 512 (Thread 0 Subpool 2)
[allocate] adding new subpool with size 512 (Thread 0 Subpool 2)
[allocate] subpool 2 has 28 bytes left @ 00000161EA962154 (Thread 0)
[allocate] subpool 2 has 28 bytes left @ 00000161EA962154 (Thread 0)
[allocate] allocated 48 bytes @ 00000161EA977110 (Thread 0 Subpool 3)
[allocate] allocated 48 bytes @ 00000161EA977110 (Thread 0 Subpool 3)
[allocate] allocated 36 bytes @ 00000161EA977140 (Thread 0 Subpool 3)
[allocate] allocated 36 bytes @ 00000161EA977140 (Thread 0 Subpool 3)
[deallocate] deallocated 44 bytes @ 00000161EA9620F8 (Thread 0 Subpool 3)
[deallocate] deallocated 44 bytes @ 00000161EA9620F8 (Thread 0 Subpool 3)
[deallocate] merged 00000161EA962124 (48 bytes) and 00000161EA962154 (28 bytes) into 76 bytes (Thread 0)
[deallocate] merged 00000161EA962124 (48 bytes) and 00000161EA962154 (28 bytes) into 76 bytes (Thread 0)
[deallocate] merged 00000161EA9620F8 (44 bytes) and 00000161EA962124 (76 bytes) into 120 bytes (Thread 0)
[deallocate] merged 00000161EA9620F8 (44 bytes) and 00000161EA962124 (76 bytes) into 120 bytes (Thread 0)
[deallocate] merged 00000161EA9620D4 (36 bytes) and 00000161EA9620F8 (120 bytes) into 156 bytes (Thread 0)
[deallocate] merged 00000161EA9620D4 (36 bytes) and 00000161EA9620F8 (120 bytes) into 156 bytes (Thread 0)
[deallocate] deallocated 24 bytes @ 00000161EA904D08 (Thread 0 Subpool 3)
[deallocate] deallocated 24 bytes @ 00000161EA904D08 (Thread 0 Subpool 3)
[deallocate] deallocated 36 bytes @ 00000161EA977140 (Thread 0 Subpool 3)
[deallocate] deallocated 36 bytes @ 00000161EA977140 (Thread 0 Subpool 3)
[deallocate] merged 00000161EA977110 (48 bytes) and 00000161EA977140 (36 bytes) into 84 bytes (Thread 0)
[deallocate] merged 00000161EA977110 (48 bytes) and 00000161EA977140 (36 bytes) into 84 bytes (Thread 0)
[deallocate] merged 00000161EA904CD0 (56 bytes) and 00000161EA904D08 (24 bytes) into 80 bytes (Thread 0)
[deallocate] merged 00000161EA904CD0 (56 bytes) and 00000161EA904D08 (24 bytes) into 80 bytes (Thread 0)
[deallocate] merged 00000161EA9620A0 (52 bytes) and 00000161EA9620D4 (156 bytes) into 208 bytes (Thread 0)
[deallocate] merged 00000161EA9620A0 (52 bytes) and 00000161EA9620D4 (156 bytes) into 208 bytes (Thread 0)
[deallocate] merged 00000161EA904CA0 (48 bytes) and 00000161EA904CD0 (80 bytes) into 128 bytes (Thread 0)
[deallocate] merged 00000161EA904CA0 (48 bytes) and 00000161EA904CD0 (80 bytes) into 128 bytes (Thread 0)
[deallocate] merged 00000161EA962070 (48 bytes) and 00000161EA9620A0 (208 bytes) into 256 bytes (Thread 0)
[deallocate] merged 00000161EA962070 (48 bytes) and 00000161EA9620A0 (208 bytes) into 256 bytes (Thread 0)
[gc]: subpool 1 released (128 bytes)
[gc]: subpool 1 released (128 bytes)
[gc]: subpool 2 released (256 bytes)
[gc]: subpool 2 released (256 bytes)
mem_freed: 384 bytes
Memory Pool:  Usage: 0 of 512 bytes (0%)
  Subpools:
    1: released by gc
    2: released by gc
    3: 00000161EA977110 ~ 00000161EA977310 (512 bytes)
      Position @ 00000161EA977164
      428 bytes unused

  Deallocated blocks:
    00000161EA977110: 84 bytes
      Subpool: 3, type: int

[allocate] reused freed block of 84 bytes and has 68 bytes left @ 00000161EA977154 (Thread 0 Subpool 3)
[allocate] reused freed block of 84 bytes and has 68 bytes left @ 00000161EA977154 (Thread 0 Subpool 3)
Memory Pool:  Usage: 16 of 512 bytes (3%)
  Subpools:
    1: released by gc
    2: released by gc
    3: 00000161EA977110 ~ 00000161EA977310 (512 bytes)
      Position @ 00000161EA977164
      428 bytes unused

  Deallocated blocks:
    00000161EA977110: 68 bytes
      Subpool: 3, type: int

[pool] pool of 128 bytes constructed
[pool] pool of 128 bytes constructed
[allocate] allocated 56 bytes @ 00000161EA904CA0 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 00000161EA904CA0 (Thread 0 Subpool 1)
0, 1, 2, 3, 4,
Memory Pool:  Usage: 56 of 128 bytes (43%)
  Subpools:
    1: 00000161EA904CA0 ~ 00000161EA904D20 (128 bytes)
      Position @ 00000161EA904CD8
      72 bytes unused

  Deallocated blocks:

[deallocate] deallocated 536 bytes @ 00000161EA976E48 (Thread 0 Subpool 3)
[deallocate] deallocated 536 bytes @ 00000161EA976E48 (Thread 0 Subpool 3)
[deallocate] merged 00000161EA961F30 (32 bytes) and 00000161EA961F50 (224 bytes) into 256 bytes (Thread 0)
[deallocate] merged 00000161EA961F30 (32 bytes) and 00000161EA961F50 (224 bytes) into 256 bytes (Thread 0)
[deallocate] merged 00000161EA976CD0 (376 bytes) and 00000161EA976E48 (536 bytes) into 912 bytes (Thread 0)
[deallocate] merged 00000161EA976CD0 (376 bytes) and 00000161EA976E48 (536 bytes) into 912 bytes (Thread 0)
*/