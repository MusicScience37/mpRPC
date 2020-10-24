option(STL_EXT_BENCH_IN_TEST "perform benchmarks in tests" OFF)
if(STL_EXT_BENCH_IN_TEST)
    set(STL_EXT_BENCH_DIR
        "${CMAKE_BINARY_DIR}/bench"
        CACHE PATH "directory to which benchmark results are outputted" FORCE)
    file(MAKE_DIRECTORY ${STL_EXT_BENCH_DIR})
endif()

macro(add_stl_ext_bench _TARGET)
    target_link_libraries(${_TARGET} PRIVATE stl_ext stl_ext_cpp_warnings
                                             benchmark_main)
    target_check_clang_tidy(${_TARGET})

    if(STL_EXT_BENCH_IN_TEST)
        add_test(
            NAME ${_TARGET}
            COMMAND ${_TARGET} "--benchmark_out_format=json"
                    "--benchmark_out=${_TARGET}.json"
            WORKING_DIRECTORY ${STL_EXT_BENCH_DIR})
    endif()
endmacro()

add_executable(stl_ext_bench_shared_any shared_any_test.cpp)
add_stl_ext_bench(stl_ext_bench_shared_any)

add_executable(stl_ext_bench_shared_function shared_function_test.cpp)
add_stl_ext_bench(stl_ext_bench_shared_function)

add_executable(stl_ext_bench_pod_vector pod_vector_test.cpp)
add_stl_ext_bench(stl_ext_bench_pod_vector)
