cmake_minimum_required(VERSION 3.12)

project(
    mprpc
    VERSION 0.1
    DESCRIPTION "An implementation of MessagePack-RPC in C++"
    LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(TargetLinkSystemLibrary)
include(ConfigureClangTidy)
include(CppWarningFlags)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # ignore some flags which cannot be fixed
    target_compile_options(mprpc_cpp_warnings INTERFACE -Wno-switch
                                                        -Wno-unused-parameter)
endif()

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

add_library(asio INTERFACE)
target_include_directories(
    asio SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extern/asio/asio/include)

add_subdirectory(extern/cpp-stl-ext)

set(ZSTD_MULTITHREAD_SUPPORT
    OFF
    CACHE BOOL "disable multithread of zstd" FORCE)
set(ZSTD_BUILD_TESTS
    OFF
    CACHE BOOL "disable test of zstd" FORCE)
add_subdirectory(extern/zstd/build/cmake EXCLUDE_FROM_ALL)
target_include_directories(
    libzstd_shared
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern/zstd/lib>)

option(MPRPC_BUILD_DOC "build documentation of mpRPC" OFF)
if(MPRPC_BUILD_DOC)
    add_subdirectory(doc)
endif()

add_library(
    mprpc SHARED
    src/format_message.cpp
    src/logging/spdlog_logger.cpp
    src/thread_pool.cpp
    src/transport/parsers/msgpack_parser.cpp
    src/transport/compressors/zstd_compressor_config.cpp
    src/transport/compressors/zstd_compressor.cpp
    src/transport/parsers/zstd_parser.cpp
    src/transport/tcp/tcp.cpp
    src/transport/udp/udp.cpp)
target_include_directories(mprpc PUBLIC ${MPRPC_SOURCE_DIR}/include
                                        ${CMAKE_BINARY_DIR}/include)
set_target_properties(
    mprpc
    PROPERTIES CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN 1
               POSITION_INDEPENDENT_CODE ON)
include(GenerateExportHeader)
generate_export_header(
    mprpc EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/include/mprpc/mprpc_export.h")
target_link_libraries(mprpc PUBLIC msgpackc-cxx fmt spdlog asio stl_ext)
target_link_libraries(mprpc PRIVATE mprpc_cpp_warnings libzstd_shared)
target_check_clang_tidy(mprpc)

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)

include(CTest)

option(MPRPC_TESTING "enable testing of mpRPC" OFF)
if(MPRPC_TESTING)
    add_subdirectory(extern/Catch2)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
                   ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
    add_subdirectory(test)
endif()

option(MPRPC_ENABLE_BENCH "enable benchmarks of mpRPC" OFF)
if(MPRPC_ENABLE_BENCH)
    set(BENCHMARK_ENABLE_TESTING
        OFF
        CACHE BOOL "disable test of benchmark" FORCE)
    add_subdirectory(extern/benchmark)
    add_subdirectory(bench)
endif()

option(MPRPC_BUILD_EXAMPLE "build examples of mpRPC" OFF)
if(MPRPC_TESTING)
    add_subdirectory(example)
endif()
