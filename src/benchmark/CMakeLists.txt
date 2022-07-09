cmake_minimum_required(VERSION 3.0.0)
project(qalloc_benchmark VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /analyze /sdl /O2 /GF /Gy /GL /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /nologo /LTCG /OPT:REF,ICF")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

find_package(benchmark CONFIG REQUIRED)

add_definitions(-DQALLOC_BENCHMARK -DNDEBUG)
include_directories(../../include)
include_directories(${benchmark_INCLUDE_DIRS})
file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "../../include/*.hpp")
add_executable(qalloc_benchmark ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(qalloc_benchmark PRIVATE benchmark::benchmark benchmark::benchmark_main)