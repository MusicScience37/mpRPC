set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

pybind11_add_module(
    mprpc_python_wrapper
    src/mprpc/bind_logging.cpp
    src/mprpc/bind_message.cpp
    src/mprpc/bind_error_codes.cpp
    src/mprpc/bind_error.cpp
    src/mprpc/bind_config.cpp
    src/mprpc/bind.cpp)
target_include_directories(mprpc_python_wrapper
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mprpc_python_wrapper PRIVATE mprpc)
set_target_properties(mprpc_python_wrapper PROPERTIES OUTPUT_NAME _mprpc_cpp)
target_check_clang_tidy(mprpc_python_wrapper)
