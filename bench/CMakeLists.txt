add_subdirectory(utils)

set(MPRPC_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which result of benchmarks are outputted" FORCE)
file(MAKE_DIRECTORY ${MPRPC_BENCH_DIR})

option(MPRPC_BENCH_IN_TEST "perform benchmarks in tests" OFF)

macro(add_mprpc_bench _TARGET)
    target_link_libraries(
        ${_TARGET} PRIVATE benchmark_main mprpc mprpc_bench_utils
                           mprpc_cpp_warnings)
    target_check_clang_tidy(${_TARGET})

    set(BENCH_RESULT_PATH ${MPRPC_BENCH_DIR}/${_TARGET}.json)
    if(MPRPC_BENCH_IN_TEST)
        add_test(
            NAME ${_TARGET}
            COMMAND ${_TARGET} "--benchmark_out_format=json"
                    "--benchmark_out=${_TARGET}.json"
            WORKING_DIRECTORY "${MPRPC_BENCH_DIR}")
    endif()
endmacro()

add_subdirectory(parser)
add_subdirectory(transport)
add_subdirectory(buffer)
add_subdirectory(rpc)
