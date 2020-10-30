message("Configure documentation of mpRPC")

add_custom_target(mprpc_doc)

option(MPRPC_BUILD_DOC_DOXYGEN "build documentation of mpRPC with Doxygen" ON)
if(MPRPC_BUILD_DOC_DOXYGEN)
    add_subdirectory(doxygen)
endif()

option(MPRPC_BUILD_DOC_SPHINX "build documentation of mpRPC with Sphinx" ON)
if(MPRPC_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()
