option(STL_EXT_WRITE_JUNIT "write JUnit XML files in tests" OFF)
if(STL_EXT_WRITE_JUNIT)
    set(STL_EXT_JUNIT_DIR
        "${CMAKE_BINARY_DIR}/junit"
        CACHE PATH "directory to which JUnit XML files are outputted" FORCE)
    file(MAKE_DIRECTORY ${STL_EXT_JUNIT_DIR})
endif()

set(STL_EXT_TEMP_TEST_DIR
    "${CMAKE_BINARY_DIR}/temp_test"
    CACHE PATH "directory used in test" FORCE)
file(MAKE_DIRECTORY ${STL_EXT_TEMP_TEST_DIR})

macro(target_add_catch2 _TARGET)
    target_link_libraries(${_TARGET} PRIVATE stl_ext stl_ext_cpp_warnings
                                             Catch2WithMain)
    target_check_clang_tidy(${_TARGET})

    if(STL_EXT_WRITE_JUNIT)
        set(_CATCH2_REPORT_OPTIONS -r junit -o
                                   ${STL_EXT_JUNIT_DIR}/${_TARGET}.xml)
    else()
        set(_CATCH2_REPORT_OPTIONS -r compact)
    endif()

    add_test(
        NAME ${_TARGET}
        COMMAND ${_TARGET} ${_CATCH2_REPORT_OPTIONS}
        WORKING_DIRECTORY ${STL_EXT_TEMP_TEST_DIR})
endmacro()

add_subdirectory(units)
