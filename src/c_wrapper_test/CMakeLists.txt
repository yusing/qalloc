cmake_minimum_required(VERSION 3.0.0)
project(qalloc_c_wrapper_test VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /analyze /sdl /Zc:__cplusplus")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

add_definitions(-DQALLOC_GTEST)
include(CTest)
enable_testing()

find_package(GTest CONFIG REQUIRED)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include_directories(../../include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../qalloc_c_wrapper)
include_directories(${GTest_INCLUDE_DIRECTORIES})

link_directories(${CMAKE_LINK_DIRECTORIES_BEFORE} ${CMAKE_CURRENT_BINARY_DIR}/../qalloc_c_wrapper)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "../../include/*.hpp")
add_executable(qalloc_c_wrapper_test ${SRC_FILES} ${HEADER_FILES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(qalloc_c_wrapper_test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main qalloc_c_wrapper)

add_dependencies(qalloc_c_wrapper_test qalloc_c_wrapper)