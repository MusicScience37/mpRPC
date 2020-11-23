cmake_minimum_required(VERSION 3.12)

project(
    mprpc
    VERSION 0.3
    DESCRIPTION "An implementation of MessagePack-RPC in C++"
    LANGUAGES CXX)

option(MPRPC_BUILD_DOC "build documentation of mpRPC" OFF)
option(MPRPC_TESTING "enable testing of mpRPC" OFF)
option(MPRPC_ENABLE_BENCH "enable benchmarks of mpRPC" OFF)
option(MPRPC_BUILD_EXAMPLE "build examples of mpRPC" OFF)
option(MPRPC_PYTHON_INTERFACE "enable Python interface of mpRPC" OFF)

set(CMAKE_CXX_STANDARD
    14
    CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED
    ON
    CACHE BOOL "require standard")
set(CMAKE_CXX_EXTENSIONS
    ON
    CACHE BOOL "use compiler specific extensions")

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

add_subdirectory(extern/toml11)

find_package(Threads REQUIRED)

if(MPRPC_BUILD_DOC
   OR MPRPC_TESTING
   OR MPRPC_ENABLE_BENCH
   OR MPRPC_PYTHON_INTERFACE)
    find_program(PIPENV_EXECUTABLE NAMES pipenv)
    if(NOT PIPENV_EXECUTABLE)
        message(
            FATAL_ERROR
                "pipenv is required for MPRPC_BUILD_DOC, MPRPC_TESTING, MPRPC_ENABLE_BENCH or MPRPC_PYTHON_INTERFACE"
        )
    endif()
    message(STATUS "Found pipenv: ${PIPENV_EXECUTABLE}")
    execute_process(
        COMMAND ${PIPENV_EXECUTABLE} --venv
        WORKING_DIRECTORY ${MPRPC_SOURCE_DIR}
        OUTPUT_VARIABLE PIPENV_VENV_DIR)
    string(REGEX REPLACE "\n$" "" PIPENV_VENV_DIR "${PIPENV_VENV_DIR}")
    message(STATUS "Virtualenv location: ${PIPENV_VENV_DIR}")
endif()

if(MPRPC_BUILD_DOC)
    add_subdirectory(doc)
endif()

add_library(mprpc_pch INTERFACE)
if(NOT CMAKE_VERSION VERSION_LESS 3.16)
    option(MPRPC_USE_PCH "use precompiled headers in mpRPC" ON)
    if(MPRPC_USE_PCH)
        target_link_libraries(mprpc_pch INTERFACE msgpackc-cxx)
        target_precompile_headers(mprpc_pch INTERFACE <msgpack.hpp>)
    endif()
endif()

add_library(
    mprpc SHARED
    src/shared_binary.cpp
    src/format_message.cpp
    src/logging/basic_loggers.cpp
    src/thread_pool.cpp
    src/transport/parsers/msgpack_parser.cpp
    src/transport/compressors/zstd_compressor_config.cpp
    src/transport/compressors/zstd_compressor.cpp
    src/transport/parsers/zstd_parser.cpp
    src/transport/tcp/tcp.cpp
    src/transport/udp/udp.cpp
    src/config/load.cpp)
target_include_directories(mprpc PUBLIC ${MPRPC_SOURCE_DIR}/include
                                        ${CMAKE_BINARY_DIR}/include)
target_include_directories(mprpc PRIVATE ${MPRPC_SOURCE_DIR}/impl)
set_target_properties(
    mprpc
    PROPERTIES CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN 1
               POSITION_INDEPENDENT_CODE ON)
include(GenerateExportHeader)
generate_export_header(
    mprpc EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/include/mprpc/mprpc_export.h")
target_link_libraries(mprpc PUBLIC msgpackc-cxx fmt asio stl_ext
                                   Threads::Threads toml11::toml11)
target_link_libraries(mprpc PRIVATE mprpc_cpp_warnings mprpc_pch spdlog
                                    libzstd_shared)
set_target_properties(mprpc PROPERTIES VERSION ${PROJECT_VERSION})
target_check_clang_tidy(mprpc)

add_library(mprpc_impl INTERFACE)
target_link_libraries(mprpc_impl INTERFACE mprpc)
target_include_directories(mprpc_impl INTERFACE ${MPRPC_SOURCE_DIR}/impl)

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
include(CTest)

if(MPRPC_TESTING OR MPRPC_ENABLE_BENCH)
    add_subdirectory(extern/Catch2)
    add_subdirectory(test)
endif()

if(MPRPC_ENABLE_BENCH)
    set(BENCHMARK_ENABLE_TESTING
        OFF
        CACHE BOOL "disable test of benchmark" FORCE)
    add_subdirectory(extern/benchmark)
    add_subdirectory(bench)
endif()

if(MPRPC_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(MPRPC_PYTHON_INTERFACE)
    find_program(
        PYTHON_EXECUTABLE
        NAMES python3
        PATHS ${PIPENV_VENV_DIR}/bin ${PIPENV_VENV_DIR}/Scripts
        NO_DEFAULT_PATH)
    if(NOT PYTHON_EXECUTABLE)
        message(FATAL_ERROR "Python not found for Python interface")
    endif()
    message(STATUS "Found Python: ${PYTHON_EXECUTABLE}")
    add_subdirectory(extern/pybind11)
    add_subdirectory(python)
endif()
