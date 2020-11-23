add_subdirectory(mprpc_cpp)

if(WIN32)
    set(PATH_SEPARATOR ";")
else()
    set(PATH_SEPARATOR ":")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.env.in
               ${CMAKE_CURRENT_SOURCE_DIR}/../.env)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.pylintrc.in
               ${CMAKE_CURRENT_SOURCE_DIR}/../.pylintrc)

if(MPRPC_TESTING)
    add_test(
        NAME mprpc_python_test
        COMMAND
            ${PIPENV_EXECUTABLE} run pytest
            --junitxml=${MPRPC_JUNIT_DIR}/mprpc_python_test.xml --cov=mprpc
            --cov-report html:${CMAKE_BINARY_DIR}/coverage/python/html
            ${CMAKE_CURRENT_SOURCE_DIR}/test
        WORKING_DIRECTORY ${MPRPC_SOURCE_DIR})
endif()
