message("Configure documentation of mpRPC")

add_custom_target(mprpc_doc)

add_subdirectory(doxygen)
add_subdirectory(sphinx)
