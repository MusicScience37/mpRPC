add_subdirectory(utils)

set(MPRPC_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which result of benchmarks are outputted" FORCE)
file(MAKE_DIRECTORY ${MPRPC_BENCH_DIR})

add_custom_target(mprpc_bench COMMENT "executed benchmarks of mpRPC")

macro(add_mprpc_bench _TARGET)
    target_link_libraries(
        ${_TARGET} PRIVATE benchmark_main mprpc mprpc_bench_utils
                           mprpc_cpp_warnings)
    target_check_clang_tidy(${_TARGET})

    set(BENCH_RESULT_PATH ${MPRPC_BENCH_DIR}/${_TARGET}.json)
    add_custom_target(
        ${_TARGET}_execution
        COMMAND ${_TARGET} "--benchmark_out_format=json"
                "--benchmark_out=${_TARGET}.json"
        DEPENDS ${_TARGET}
        WORKING_DIRECTORY "${MPRPC_BENCH_DIR}"
        COMMENT "executing ${_TARGET}")
    add_dependencies(mprpc_bench ${_TARGET}_execution)
endmacro()

add_subdirectory(parser)
