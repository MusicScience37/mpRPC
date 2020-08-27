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

add_library(mprpc INTERFACE)
target_include_directories(mprpc INTERFACE ${MPRPC_SOURCE_DIR}/include)

option(MPRPC_TESTING "enable testing of mpRPC" OFF)
if(MPRPC_TESTING)
    add_subdirectory(extern/Catch2)
    enable_testing()
    add_subdirectory(test)
endif()
