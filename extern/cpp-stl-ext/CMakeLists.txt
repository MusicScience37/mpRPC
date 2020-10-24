cmake_minimum_required(VERSION 3.12)

project(
    stl_ext
    VERSION 0.2
    DESCRIPTION "Extension for STL in C++"
    LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(TargetLinkSystemLibrary)
include(ConfigureClangTidy)
include(CppWarningFlags)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

set(STL_EXT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(STL_EXT_BUILD_DOC "build documentation of stl_ext" OFF)
if(STL_EXT_BUILD_DOC)
    add_subdirectory(doc)
endif()

add_library(stl_ext INTERFACE)
target_include_directories(stl_ext INTERFACE ${STL_EXT_SOURCE_DIR}/include)

option(STL_EXT_TESTING "enable testing of stl_ext" OFF)
if(STL_EXT_TESTING)
    add_subdirectory(extern/Catch2)
    include(CTest)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
                   ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
    add_subdirectory(test)
endif()

option(STL_EXT_ENABLE_BENCH "enable benchmarks of stl_ext" OFF)
if(STL_EXT_ENABLE_BENCH)
    set(BENCHMARK_ENABLE_TESTING
        OFF
        CACHE BOOL "disable test of benchmark" FORCE)
    add_subdirectory(extern/benchmark)
    add_subdirectory(bench)
endif()
