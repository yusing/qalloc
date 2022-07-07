cmake_minimum_required(VERSION 3.0.0)
project(qalloc_c_wrapper VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -g -O0")
endif()

include_directories(../../include)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "../../include/*.hpp" "../../include/*.h")
add_library(qalloc_c_wrapper STATIC ${SRC_FILES} ${HEADER_FILES})