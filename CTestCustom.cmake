set(CTEST_CUSTOM_MEMCHECK_IGNORE mprpc_test_integ_logging_write_log
                                 mprpc_bench_graph_echo
                                 mprpc_python_test)
set(CTEST_CUSTOM_COVERAGE_EXCLUDE "(extern|test)/")
