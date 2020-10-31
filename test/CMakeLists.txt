add_library(mprpc_catch2_main OBJECT catch2_main.cpp)
target_link_system_library(mprpc_catch2_main PUBLIC Catch2::Catch2)
target_check_clang_tidy(mprpc_catch2_main)

add_subdirectory(utils)

option(MPRPC_WRITE_JUNIT "write JUnit XML files in tests" OFF)
if(MPRPC_WRITE_JUNIT)
    set(MPRPC_JUNIT_DIR
        "${CMAKE_BINARY_DIR}/junit"
        CACHE PATH "directory to which JUnit XML files are outputted" FORCE)
    file(MAKE_DIRECTORY ${MPRPC_JUNIT_DIR})
endif()

set(MPRPC_TEMP_TEST_DIR
    "${CMAKE_BINARY_DIR}/temp_test"
    CACHE PATH "directory used in test" FORCE)
file(MAKE_DIRECTORY ${MPRPC_TEMP_TEST_DIR})

macro(target_add_catch2 _TARGET)
    target_link_libraries(
        ${_TARGET} PRIVATE mprpc_catch2_main mprpc mprpc_test_utils
                           mprpc_cpp_warnings)
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
