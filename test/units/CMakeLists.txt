add_executable(mprpc_test_units message_data_test.cpp error_info_test.cpp
                                exception_test.cpp)
target_add_catch2(mprpc_test_units)
