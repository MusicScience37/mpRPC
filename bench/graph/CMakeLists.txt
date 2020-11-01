find_package(Python3 COMPONENTS Interpreter)

macro(add_mprpc_bench_graph _TARGET _SCRIPT)
    target_link_libraries(${_TARGET} PRIVATE mprpc mprpc_bench_utils
                                             mprpc_cpp_warnings lyra)
    target_check_clang_tidy(${_TARGET})

    set(BENCH_RESULT_PATH ${MPRPC_BENCH_DIR}/${_TARGET}.json)
    if(MPRPC_BENCH_IN_TEST)
        add_test(
            NAME ${_TARGET}
            COMMAND ${Python3_EXECUTABLE}
                    "${CMAKE_CURRENT_SOURCE_DIR}/${_SCRIPT}" ${CMAKE_BINARY_DIR}
            WORKING_DIRECTORY "${MPRPC_BENCH_DIR}")
    endif()
endmacro()

add_executable(mprpc_bench_graph_echo echo_test.cpp)
add_mprpc_bench_graph(mprpc_bench_graph_echo echo_grpah.py)
