cmake_minimum_required(VERSION 3.0.0)
project(qalloc_c_wrapper VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /analyze /sdl /Zc:__cplusplus")
endif()

if (${CMAKE_BUILD_TYPE} MATCHES "Release")
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Gy /Ot /Ob2 /GR-")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()
endif()

include_directories(../../include)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "../../include/*.hpp" "../../include/*.h")

#set(CMAKE_CXX_VISIBILITY_PRESET default)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN 0)
include(GenerateExportHeader)
add_library(qalloc_c_wrapper STATIC ${SRC_FILES} ${HEADER_FILES})
generate_export_header(qalloc_c_wrapper BASE_NAME qalloc EXPORT_MACRO_NAME QALLOC_EXPORT EXPORT_FILE_NAME qalloc_export.h NO_EXPORT_MACRO_NAME QALLOC_NO_EXPORT)
target_include_directories(qalloc_c_wrapper BEFORE PUBLIC ${CMAKE_CURRENT_BINARY_DIR})