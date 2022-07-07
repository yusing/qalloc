
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

/// @file benchmark.cpp
/// @brief qalloc Google Benchmark file.
/// @author yusing
/// @date 2022-07-02

#include <vector>
#include <string>
#include <unordered_map>
#include <qalloc/qalloc.hpp>
#include <benchmark/benchmark.h>

template <typename TestVector, typename Element>
static void v_emplace_reset(TestVector& v, const Element& e) {
    for (int i = 0; i < 100; i++) {
        v.emplace_back(e);
    }
    v = TestVector();
}

template <typename TestString>
static void string_append_reset(TestString& s) {
    for (int i = 0; i < 100; i++) {
        s.append("Hello, world!\n");
    }
    s = TestString();
}

template <typename TestMap>
static void m_insert_reset(TestMap& m) {
    for (int i = 0; i < 100; ++i) {
        m[i] = i;
    }
    m = TestMap();
}

template <typename TestList>
static void list_emplace_reset(TestList& l) {
    for (int i = 0; i < 100; i++) {
        l.emplace_back(i);
    }
    l = TestList();
}

static void Std_Vector_String_Emplace_Reset(benchmark::State& state) {
    std::vector<std::string> v;
    for (auto _ : state) {
        v_emplace_reset(v, "Hello, world!\n");
    }
}

static void QAlloc_Vector_String_Emplace_Reset(benchmark::State& state) {
    qalloc::vector<qalloc::string> v;
    for (auto _ : state) {
        v_emplace_reset(v, "Hello, world!\n");
    }
}

static void Std_Vector_QAlloc_String_Emplace_Reset(benchmark::State& state) {
    std::vector<qalloc::string> v;
    for (auto _ : state) {
        v_emplace_reset(v, "Hello, world!\n");
    }
}

static void QAlloc_Vector_Std_String_Emplace_Reset(benchmark::State& state) {
    qalloc::vector<std::string> v;
    for (auto _ : state) {
        v_emplace_reset(v, "Hello, world!\n");
    }
}

static void Std_Vector_Int_Emplace_Reset(benchmark::State& state) {
    std::vector<int> v;
    for (auto _ : state) {
        v_emplace_reset(v, 0);
    }
}

static void QAlloc_Vector_Int_Emplace_Reset(benchmark::State& state) {
    qalloc::vector<int> v;
    for (auto _ : state) {
        v_emplace_reset(v, 0);
    }
}

static void Std_String_Creation(benchmark::State& state) {
    std::string s = "Hello";
    for (auto _ : state) {
        std::string copy(s);
        (void)copy;
    }
}

static void QAlloc_String_Creation(benchmark::State& state) {
    qalloc::string s;
    for (auto _ : state) {
        qalloc::string copy(s);
        (void)copy;
    }
}

static void Std_String_Append_Reset(benchmark::State& state) {
    std::string s;
    for (auto _ : state) {
        string_append_reset(s);
    }
}

static void QAlloc_String_Append_Reset(benchmark::State& state) {
    qalloc::string s;
    for (auto _ : state) {
        string_append_reset(s);
    }
}

static void Std_Unordered_Map_Int_Int_Insert_Reset(benchmark::State& state) {
    std::unordered_map<int, int> m;
    for (auto _ : state) {
        m_insert_reset(m);
    }
}

static void QAlloc_Unordered_Map_Int_Int_Insert_Reset(benchmark::State& state) {
    qalloc::unordered_map<int, int> m;
    for (auto _ : state) {
        m_insert_reset(m);
    }
}

static void Std_List_Double_Emplace_Reset(benchmark::State& state) {
    std::list<double> l;
    for (auto _ : state) {
        list_emplace_reset(l);
    }
}

static void QAlloc_List_Double_Emplace_Reset(benchmark::State& state) {
    qalloc::list<double> l;
    for (auto _ : state) {
        list_emplace_reset(l);
    }
}

BENCHMARK(Std_Vector_String_Emplace_Reset);
BENCHMARK(Std_Vector_QAlloc_String_Emplace_Reset);
BENCHMARK(QAlloc_Vector_Std_String_Emplace_Reset);
BENCHMARK(QAlloc_Vector_String_Emplace_Reset);
BENCHMARK(Std_Vector_Int_Emplace_Reset);
BENCHMARK(QAlloc_Vector_Int_Emplace_Reset);
BENCHMARK(Std_String_Creation);
BENCHMARK(QAlloc_String_Creation);
BENCHMARK(Std_String_Append_Reset);
BENCHMARK(QAlloc_String_Append_Reset);
BENCHMARK(Std_Unordered_Map_Int_Int_Insert_Reset);
BENCHMARK(QAlloc_Unordered_Map_Int_Int_Insert_Reset);
BENCHMARK(Std_List_Double_Emplace_Reset);
BENCHMARK(QAlloc_List_Double_Emplace_Reset);

BENCHMARK_MAIN();