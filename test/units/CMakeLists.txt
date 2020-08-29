add_executable(
    mprpc_test_units
    logging/log_data_traits_test.cpp logging/format_log_data_test.cpp
    message_data_test.cpp error_info_test.cpp exception_test.cpp
    format_message_test.cpp)
target_add_catch2(mprpc_test_units)
