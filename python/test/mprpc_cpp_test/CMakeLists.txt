set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

pybind11_add_module(mprpc_python_test_wrapper src/bind.cpp)
target_include_directories(
    mprpc_python_test_wrapper
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../../mprpc_cpp/include)
target_link_libraries(mprpc_python_test_wrapper PRIVATE mprpc mprpc_test_utils)
set_target_properties(mprpc_python_test_wrapper PROPERTIES OUTPUT_NAME
                                                           mprpc_cpp_test)
target_check_clang_tidy(mprpc_python_test_wrapper)
