add_subdirectory(utils)

option(MPRPC_WRITE_JUNIT "write JUnit XML files in tests" OFF)

set(MPRPC_TEMP_TEST_DIR
    "${CMAKE_BINARY_DIR}/temp_test"
    CACHE PATH "directory used in test" FORCE)
file(MAKE_DIRECTORY ${MPRPC_TEMP_TEST_DIR})

macro(target_add_catch2 _TARGET)
    target_link_libraries(
        ${_TARGET} PRIVATE Catch2::Catch2WithMain mprpc mprpc_test_utils
                           mprpc_cpp_warnings mprpc_pch)
    target_check_clang_tidy(${_TARGET})

    if(MPRPC_WRITE_JUNIT)
        set(_CATCH2_REPORT_OPTIONS -r junit -o
                                   ${MPRPC_JUNIT_DIR}/${_TARGET}.xml)
    else()
        set(_CATCH2_REPORT_OPTIONS -r compact)
    endif()

    add_test(
        NAME ${_TARGET}
        COMMAND ${_TARGET} ${_CATCH2_REPORT_OPTIONS}
        WORKING_DIRECTORY ${MPRPC_TEMP_TEST_DIR})
endmacro()

add_subdirectory(units)
add_subdirectory(integration)
