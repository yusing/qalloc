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

/// @file test.cpp
/// @brief qalloc Google Test file.
/// @author yusing
/// @date 2022-07-02

#include <qalloc/qalloc.hpp>
#include <thread>
#include <gtest/gtest.h>

constexpr char DIGITS[] = "0123456789";

static void test_s() {
    qalloc::string s;
    for (int i = 0; i < 1000; i++) { // add some char
        s.push_back(DIGITS[i % 10]);
    }
    ASSERT_EQ(s.size(), 1000); // check size
    std::size_t i = 0;
    for (const auto& ch : s) { // check content
        ASSERT_EQ(ch, DIGITS[i % 10]);
        ++i;
    }
}

template <typename Vector, typename ElementDelegate>
static void test_v(ElementDelegate element_at) {
    static_assert(std::is_same<decltype(ElementDelegate()(0)), typename Vector::value_type>::value, "ElementDelegate()() must return a value of type Vector::value_type");
    Vector v;
    std::vector<typename Vector::value_type> v_std;
    for (int i = 0; i < 1000; i++) { // add some elements
        v.emplace_back(element_at(i));
        v_std.emplace_back(element_at(i));
    }
    ASSERT_EQ(v.size(), v_std.size()); // check size
    ASSERT_GE(v.capacity(), v.size()); // capacity is at least size
    for (std::size_t i = 0; i < v.size(); i++) { // check content
        ASSERT_EQ(v[i], v_std[i]);
    }
    for (int i = 0; i < 100; ++i) { // emplace some elements
        v.emplace(v.begin() + i, element_at(i));
        v_std.emplace(v_std.begin() + i, element_at(i));
    }
    ASSERT_EQ(v.size(), v_std.size()); // check size
    ASSERT_GE(v.capacity(), v.size()); // capacity is at least size
    for (std::size_t i = 0; i < v.size(); i++) { // check content
        ASSERT_EQ(v[i], v_std[i]);
    }
    for (int i = 0; i < 100; i++) { // erase some elements
        v.erase(v.begin()+i);
        v_std.erase(v_std.begin()+i);
    }
    ASSERT_EQ(v.size(), v_std.size());
    ASSERT_GE(v.capacity(), v.size()); // capacity is at least size
    for (std::size_t i = 0; i < v.size(); i++) { // check content
        ASSERT_EQ(v[i], v_std[i]);
    }
}

template <typename List, typename ElementDelegate>
static void test_list(ElementDelegate element_at) {
    using value_type = typename List::value_type;
    static_assert(std::is_same<decltype(ElementDelegate()(0)), value_type>::value, "ElementDelegate()() must return a value of type Vector::value_type");
    List l(1, value_type());
    std::list<typename List::value_type> l_std(1, value_type());
    for (int i = 0; i < 1000; i++) { // add some elements
        l.emplace_back(element_at(i));
        l_std.emplace_back(element_at(i));
    }
    ASSERT_EQ(l.size(), l_std.size()); // check size
    std::vector<value_type> copy_l(l.begin(), l.end()); // copy list to vector
    std::vector<value_type> copy_l_std(l_std.begin(), l_std.end()); // copy list to vector
    for (std::size_t i = 0; i < copy_l.size(); i++) { // check content
        ASSERT_EQ(copy_l[i], copy_l_std[i]);
    }
}

static int emplace_index(std::size_t i) {
    return static_cast<int>(i);
}


static qalloc::string emplace_index_qalloc_string(std::size_t i) {
    auto std_str = std::to_string(i);
    return {
        std_str.c_str() // NOLINT(readability-redundant-string-cstr)
    };
}

TEST(QAllocSingleThread, QAllocString) {
    test_s();
}

TEST(QAllocSingleThread, QAllocVectorInt) {
    test_v<qalloc::vector<int>>(emplace_index);
}

TEST(QAllocSingleThread, QAllocVectorQAllocString) {
    test_v<qalloc::vector<qalloc::string>>(emplace_index_qalloc_string);
}

TEST(QAllocSingleThread, QAllocListInt) {
    test_list<qalloc::list<int>>(emplace_index);
}

TEST(QAllocMultiThread, QAllocString) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; i++) {
        threads.emplace_back(test_s);
    }
    for (auto& t : threads) {
        t.join();
    }
}

TEST(QAllocMultiThread, QAllocVectorInt) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; i++) {
        threads.emplace_back([]() {
            test_v<qalloc::vector<int>>(emplace_index);
        });
    }
    for (auto& t : threads) {
        t.join();
    }
}

TEST(QAllocMultiThread, QAllocVectorQAllocString) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; i++) {
        threads.emplace_back([]() {
            test_v<qalloc::vector<qalloc::string>>(emplace_index_qalloc_string);
        });
    }
    for (auto& t : threads) {
        t.join();
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}