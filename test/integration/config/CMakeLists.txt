add_executable(mprpc_test_integ_config config_test.cpp)
target_add_catch2(mprpc_test_integ_config)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mprpc_test_integ_config.toml
               ${MPRPC_TEMP_TEST_DIR}/mprpc_test_integ_config.toml COPYONLY)
