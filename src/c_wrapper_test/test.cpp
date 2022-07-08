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

/// @file c_wrapper_test/test.cpp
/// @brief qalloc c wrapper library Google Test file.
/// @author yusing
/// @date 2022-07-08

#include <gtest/gtest.h>
#include <qalloc/qalloc.h>

TEST(QAllocCWrapper, QAllocateAssignment) {
    size_t* ui_arr = static_cast<size_t *>(q_allocate(10 * sizeof(size_t)));
    for (size_t i = 0; i < 10; ++i) {
        ui_arr[i] = i;
    }
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(ui_arr[i], i);
    }
    q_deallocate(ui_arr);
}

TEST(QAllocCWrapper, QCallocIsZeros) {
    size_t* ui_arr = static_cast<size_t *>(q_callocate(10, sizeof(size_t)));
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(ui_arr[i], 0);
    }
    q_deallocate(ui_arr);
}

TEST(QAllocCWrapper, QReallocateLarger) {
    size_t* ui_arr = static_cast<size_t *>(q_allocate(10 * sizeof(size_t)));
    for (size_t i = 0; i < 10; ++i) {
        ui_arr[i] = i;
    }
    ui_arr = static_cast<size_t *>(q_reallocate(ui_arr, 20 * sizeof(size_t)));
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(ui_arr[i], i);
    }
    q_deallocate(ui_arr);
}

TEST(QAllocCWrapper, QReallocateSmaller) {
    size_t* ui_arr = static_cast<size_t *>(q_allocate(10 * sizeof(size_t)));
    for (size_t i = 0; i < 10; ++i) {
        ui_arr[i] = i;
    }
    ui_arr = static_cast<size_t *>(q_reallocate(ui_arr, 5 * sizeof(size_t)));
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(ui_arr[i], i);
    }
    q_deallocate(ui_arr);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}