if(NOT PYTEST_BUILD_EXECUTABLE)
    return()
endif()

add_executable(mprpc_test_integ_logging_write_log write_log.cpp)
target_link_libraries(mprpc_test_integ_logging_write_log mprpc lyra
                      mprpc_cpp_warnings mprpc_pch)
target_check_clang_tidy(mprpc_test_integ_logging_write_log)

add_test(
    NAME mprpc_test_integ_logging_write_log
    COMMAND ${PYTEST_BUILD_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR} --writer
            $<TARGET_FILE:mprpc_test_integ_logging_write_log>
    WORKING_DIRECTORY ${MPRPC_TEMP_TEST_DIR})
