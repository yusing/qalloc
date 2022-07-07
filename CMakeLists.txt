cmake_minimum_required(VERSION 3.0.0)
project(qalloc VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

add_executable(qalloc example.cpp)

add_subdirectory(src/test)
add_subdirectory(src/benchmark)
add_subdirectory(src/c_wrapper)