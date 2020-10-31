find_program(
    PYTEST_BUILD_EXECUTABLE
    NAMES pytest
    PATHS ${MPRPC_SOURCE_DIR}/.venv/bin ${MPRPC_SOURCE_DIR}/.venv/Scripts
    NO_DEFAULT_PATH)

if(NOT PYTEST_BUILD_EXECUTABLE)
    find_program(PYTEST_BUILD_EXECUTABLE NAMES pytest)
endif()

if(PYTEST_BUILD_EXECUTABLE)
    message("Found pytest: ${PYTEST_BUILD_EXECUTABLE}")
else()
    message("Pytest not found, so ignoring some tests")
endif()

add_subdirectory(logging)
add_subdirectory(tcp)
add_subdirectory(udp)
