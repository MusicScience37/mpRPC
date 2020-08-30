cmake_minimum_required(VERSION 3.12)

project(
    mprpc
    VERSION 0.0
    DESCRIPTION "An implementation of MessagePack-RPC in C++"
    LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(TargetLinkSystemLibrary)
include(ConfigureClangTidy)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

set(MPRPC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(MSGPACK_ENABLE_CXX
    ON
    CACHE BOOL "use C++ version of msgpack" FORCE)
set(MSGPACK_CXX11
    ON
    CACHE BOOL "use C++ 11 version of msgpack" FORCE)
set(MSGPACK_BUILD_TESTS
    OFF
    CACHE BOOL "don't test msgpack" FORCE)
set(MSGPACK_BUILD_EXAMPLES
    OFF
    CACHE BOOL "don't build examples in msgpack" FORCE)
add_subdirectory(extern/msgpack-c EXCLUDE_FROM_ALL)

set(BUILD_SHARED_LIBS
    ON
    CACHE BOOL "build shared library")
add_subdirectory(extern/fmt)

add_subdirectory(extern/lyra)

set(SPDLOG_FMT_EXTERNAL
    ON
    CACHE BOOL "use separate fmt library in spdlog" FORCE)
set(SPDLOG_BUILD_SHARED
    ON
    CACHE BOOL "build shared library of spdlog" FORCE)
add_subdirectory(extern/spdlog EXCLUDE_FROM_ALL)

option(MPRPC_BUILD_DOC "build documentation of mpRPC" OFF)
if(MPRPC_BUILD_DOC)
    add_subdirectory(doc)
endif()

add_library(mprpc INTERFACE)
target_include_directories(mprpc INTERFACE ${MPRPC_SOURCE_DIR}/include)
target_link_libraries(mprpc INTERFACE msgpackc-cxx fmt spdlog)

option(MPRPC_TESTING "enable testing of mpRPC" OFF)
if(MPRPC_TESTING)
    add_subdirectory(extern/Catch2)
    enable_testing()
    add_subdirectory(test)
endif()

option(MPRPC_BUILD_EXAMPLE "build examples of mpRPC" OFF)
if(MPRPC_TESTING)
    add_subdirectory(example)
endif()
