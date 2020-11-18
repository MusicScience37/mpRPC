find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_executable(mprpc_bench_graph_echo_server echo_test_server.cpp)
target_link_libraries(
    mprpc_bench_graph_echo_server PRIVATE mprpc mprpc_bench_utils
                                          mprpc_cpp_warnings mprpc_pch lyra)
target_check_clang_tidy(mprpc_bench_graph_echo_server)

add_executable(mprpc_bench_graph_echo_client echo_test_client.cpp)
target_link_libraries(
    mprpc_bench_graph_echo_client PRIVATE mprpc mprpc_bench_utils
                                          mprpc_cpp_warnings mprpc_pch lyra)
target_check_clang_tidy(mprpc_bench_graph_echo_client)

if(MPRPC_BENCH_IN_TEST)
    add_test(
        NAME mprpc_bench_graph_echo
        COMMAND
            ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/echo_graph.py"
            "-b" ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY "${MPRPC_BENCH_DIR}")
endif()
