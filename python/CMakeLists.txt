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
    add_subdirectory(test)
endif()
