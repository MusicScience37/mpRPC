add_subdirectory(mprpc_cpp_test)

add_test(
    NAME mprpc_python_test
    COMMAND
        ${PIPENV_EXECUTABLE} run pytest
        --junitxml=${MPRPC_JUNIT_DIR}/mprpc_python_test.xml --cov=mprpc
        --cov-report html:${CMAKE_BINARY_DIR}/coverage/python/html
        ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${MPRPC_SOURCE_DIR})
