add_subdirectory(mprpc_cpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.env.in
               ${CMAKE_CURRENT_SOURCE_DIR}/../.env)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.pylintrc.in
               ${CMAKE_CURRENT_SOURCE_DIR}/../.pylintrc)
