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
    void_ptr_t p_v = v.data();
    std::cout << demangled_type_name_of(p_v) << std::endl; // std::basic_string<char, std::char_traits<char>, allocator<char>>

    // Example 2: use qalloc:pool_t
    constexpr size_t TEST_SIZE = 10_z;
    std::default_random_engine generator(1000_z); // NOLINT(cert-msc51-cpp)
    std::uniform_int_distribution<size_t> distribution(1_z,TEST_SIZE);

    auto* new_pool = new pool_t(128_z); // allocate a new pool of 128 bytes
    allocator<int> allocator(new_pool); // use the new pool as allocator

    std::vector<std::pair<typename qalloc::allocator<int>::pointer, size_t>> allocated_pairs;

    for (size_t i = 0; i < TEST_SIZE; i++) { // emplace some elements
        size_t n_bytes = distribution(generator);
        (void)allocated_pairs.emplace_back(allocator.allocate(n_bytes), n_bytes);
    }
    for (size_t i = 0; i < TEST_SIZE; i++) { // deallocate all elements
        size_t idx = distribution(generator) % allocated_pairs.size();
        auto& pair = allocated_pairs[idx];
        allocator.deallocate(pair.first, pair.second);
        allocated_pairs.erase(allocated_pairs.begin() + idx);
    }
    size_t mem_freed = new_pool->gc(); // garbage collect the pool
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

    // Example 4: multi-threaded allocation
    std::vector<std::thread> threads;
    auto* i_pool = new pool_t(128_z); // allocate a new pool of 128 bytes
    qalloc::allocator<int> i_allocator(i_pool); // use the new pool as allocator
    int* int_array[16];
    int i = 0;
    for (auto & ele : int_array) {
        threads.emplace_back([&ele, &i_allocator, &i]() {
            ele = i_allocator.allocate(1);
            *ele = i++;
        });
    }
    for (auto & t : threads) {
        t.join();
    }
    for (auto & i : int_array) {
        std::cout << *i << ", ";
    }
    std::cout << std::endl;
    return 0;
}

/* Sample output:
[pool] pool of 1048576 bytes constructed
[allocate] allocated 32 bytes @ 000001E577430000 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 000001E577430020 (Thread 0 Subpool 1)
[allocate] allocated 96 bytes @ 000001E577430058 (Thread 0 Subpool 1)
[allocate] allocated 136 bytes @ 000001E5774300B8 (Thread 0 Subpool 1)
[deallocate] deallocated 96 bytes @ 000001E577430058 (Thread 0 Subpool 1)
[allocate] allocated 176 bytes @ 000001E577430140 (Thread 0 Subpool 1)
[deallocate] deallocated 136 bytes @ 000001E5774300B8 (Thread 0 Subpool 1)
[allocate] allocated 256 bytes @ 000001E5774301F0 (Thread 0 Subpool 1)
[deallocate] deallocated 176 bytes @ 000001E577430140 (Thread 0 Subpool 1)
[allocate] allocated 376 bytes @ 000001E5774302F0 (Thread 0 Subpool 1)
[deallocate] deallocated 256 bytes @ 000001E5774301F0 (Thread 0 Subpool 1)
[allocate] allocated 536 bytes @ 000001E577430468 (Thread 0 Subpool 1)
[deallocate] deallocated 376 bytes @ 000001E5774302F0 (Thread 0 Subpool 1)
class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
[pool] pool of 128 bytes constructed
[allocate] allocated 48 bytes @ 000001E577550000 (Thread 0 Subpool 1)
[allocate] allocated 56 bytes @ 000001E577550030 (Thread 0 Subpool 1)
[allocate] adding new subpool with size 256 (Thread 0 Subpool 1)
[allocate] subpool 1 has 24 bytes left @ 000001E577550068 (Thread 0)
[allocate] allocated 48 bytes @ 000001E577570000 (Thread 0 Subpool 2)
[allocate] allocated 52 bytes @ 000001E577570030 (Thread 0 Subpool 2)
[allocate] reused freed block of 24 bytes and has 0 bytes left @ 000001E577550068 (Thread 0 Subpool 2)
[allocate] allocated 36 bytes @ 000001E577570064 (Thread 0 Subpool 2)
[allocate] allocated 44 bytes @ 000001E577570088 (Thread 0 Subpool 2)
[allocate] allocated 48 bytes @ 000001E5775700B4 (Thread 0 Subpool 2)
[allocate] adding new subpool with size 512 (Thread 0 Subpool 2)
[allocate] subpool 2 has 28 bytes left @ 000001E5775700E4 (Thread 0)
[allocate] allocated 48 bytes @ 000001E577590000 (Thread 0 Subpool 3)
[allocate] allocated 36 bytes @ 000001E577590030 (Thread 0 Subpool 3)
[deallocate] deallocated 44 bytes @ 000001E577570088 (Thread 0 Subpool 3)
[deallocate] merged 000001E5775700B4 (48 bytes) and 000001E5775700E4 (28 bytes) into 76 bytes (Thread 0)
[deallocate] merged 000001E577570088 (44 bytes) and 000001E5775700B4 (76 bytes) into 120 bytes (Thread 0)
[deallocate] merged 000001E577570064 (36 bytes) and 000001E577570088 (120 bytes) into 156 bytes (Thread 0)
[deallocate] deallocated 24 bytes @ 000001E577550068 (Thread 0 Subpool 3)
[deallocate] deallocated 36 bytes @ 000001E577590030 (Thread 0 Subpool 3)
[deallocate] merged 000001E577590000 (48 bytes) and 000001E577590030 (36 bytes) into 84 bytes (Thread 0)
[deallocate] merged 000001E577550030 (56 bytes) and 000001E577550068 (24 bytes) into 80 bytes (Thread 0)
[deallocate] merged 000001E577570030 (52 bytes) and 000001E577570064 (156 bytes) into 208 bytes (Thread 0)
[deallocate] merged 000001E577550000 (48 bytes) and 000001E577550030 (80 bytes) into 128 bytes (Thread 0)
[deallocate] merged 000001E577570000 (48 bytes) and 000001E577570030 (208 bytes) into 256 bytes (Thread 0)
[gc]: subpool 1 released (128 bytes)
[gc]: subpool 2 released (256 bytes)
mem_freed: 384 bytes
Memory Pool:  Usage: 0 of 512 bytes (0%)
  Subpools:
    1: 0000000000000000 ~ 0000000000000000 (0 bytes)
      Position @ 0000000000000000
    2: 0000000000000000 ~ 0000000000000000 (0 bytes)
      Position @ 0000000000000000
    3: 000001E577590000 ~ 000001E577590200 (512 bytes)
      Position @ 000001E577590054
      428 bytes unused

  Deallocated blocks:
    000001E577590000: 84 bytes
      Subpool: 3, type: int
[allocate] reused freed block of 84 bytes and has 68 bytes left @ 000001E577590044 (Thread 0 Subpool 3)
Memory Pool:  Usage: 16 of 512 bytes (3%)
  Subpools:
    1: 0000000000000000 ~ 0000000000000000 (0 bytes)
      Position @ 0000000000000000
    2: 0000000000000000 ~ 0000000000000000 (0 bytes)
      Position @ 0000000000000000
    3: 000001E577590000 ~ 000001E577590200 (512 bytes)
      Position @ 000001E577590054
      428 bytes unused

  Deallocated blocks:
    000001E577590000: 68 bytes
      Subpool: 3, type: int
[pool] pool of 128 bytes constructed
[allocate] allocated 56 bytes @ 000001E5775A0000 (Thread 0 Subpool 1)
0, 1, 2, 3, 4,
Memory Pool:  Usage: 56 of 128 bytes (43%)
  Subpools:
    1: 000001E5775A0000 ~ 000001E5775A0080 (128 bytes)
      Position @ 000001E5775A0038
      72 bytes unused

  Deallocated blocks:

[pool] pool of 128 bytes constructed
[allocate] allocated 20 bytes @ 000001E5775D0000 (Thread 1 Subpool 1)
[allocate] allocated 20 bytes @ 000001E5775D0014 (Thread 2 Subpool 1)
[allocate] allocated 20 bytes @ 000001E5775D0028 (Thread 4 Subpool 1)
[allocate] allocated 20 bytes @ 000001E5775D003C (Thread 3 Subpool 1)
[allocate] allocated 20 bytes @ 000001E5775D0050 (Thread 5 Subpool 1)
[allocate] allocated 20 bytes @ 000001E5775D0064 (Thread 6 Subpool 1)
[allocate] adding new subpool with size 256 (Thread 7 Subpool 1)
[allocate] subpool 1 has 8 bytes left @ 000001E5775D0078 (Thread 7)
[allocate] allocated 20 bytes @ 000001E5775F0000 (Thread 7 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F0014 (Thread 8 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F0028 (Thread 9 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F003C (Thread 10 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F0050 (Thread 14 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F0064 (Thread 12 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F0078 (Thread 13 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F008C (Thread 11 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F00A0 (Thread 15 Subpool 2)
[allocate] allocated 20 bytes @ 000001E5775F00B4 (Thread 16 Subpool 2)
0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 13, 11, 12, 10, 14, 15,
[deallocate] deallocated 536 bytes @ 000001E577430468 (Thread 0 Subpool 1)
[deallocate] merged 000001E577430000 (32 bytes) and 000001E577430020 (56 bytes) into 88 bytes (Thread 0)
[deallocate] merged 000001E577430000 (88 bytes) and 000001E577430058 (96 bytes) into 184 bytes (Thread 0)
[deallocate] merged 000001E577430000 (184 bytes) and 000001E5774300B8 (136 bytes) into 320 bytes (Thread 0)
[deallocate] merged 000001E577430000 (320 bytes) and 000001E577430140 (176 bytes) into 496 bytes (Thread 0)
[deallocate] merged 000001E577430000 (496 bytes) and 000001E5774301F0 (256 bytes) into 752 bytes (Thread 0)
[deallocate] merged 000001E577430000 (752 bytes) and 000001E5774302F0 (376 bytes) into 1128 bytes (Thread 0)
[deallocate] merged 000001E577430000 (1128 bytes) and 000001E577430468 (536 bytes) into 1664 bytes (Thread 0)
 */