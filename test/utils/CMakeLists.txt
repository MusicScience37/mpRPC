add_library(mprpc_test_utils INTERFACE)
target_include_directories(mprpc_test_utils
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mprpc_test_utils INTERFACE mprpc)
