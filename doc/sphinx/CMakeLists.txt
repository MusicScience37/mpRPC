find_program(
    SPHINX_BUILD_EXECUTABLE
    NAMES sphinx-build
    PATHS ${MPRPC_SOURCE_DIR}/.venv/bin ${MPRPC_SOURCE_DIR}/.venv/Scripts
    NO_DEFAULT_PATH)

if(NOT SPHINX_BUILD_EXECUTABLE)
    find_program(SPHINX_BUILD_EXECUTABLE NAMES sphinx-build)
endif()

if(NOT SPHINX_BUILD_EXECUTABLE)
    message("Skipping Sphinx configuration")
    return()
endif()

message("Found Sphinx: ${SPHINX_BUILD_EXECUTABLE}")

option(MPRPC_BUILD_LATEX_DOC "build latex documentation of mpRPC" OFF)

if(DEFINED ENV{PLANTUML_JAR_PATH})
    set(DEFAULT_PLANTUML_JAR_PATH $ENV{PLANTUML_JAR_PATH})
else()
    set(DEFAULT_PLANTUML_JAR_PATH "")
endif()
set(PLANTUML_JAR_PATH
    "${DEFAULT_PLANTUML_JAR_PATH}"
    CACHE FILEPATH "path of plantuml.jar")

set(MPRPC_DOXYGEN_XML_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen_xml")

set(MPRPC_SPHINX_OPTIONS
    -D breathe_projects.mprpc="${MPRPC_DOXYGEN_XML_DIR}" -D
    "plantuml=java -jar ${PLANTUML_JAR_PATH}" -D release=${PROJECT_VERSION})

add_custom_target(
    mprpc_doc_sphinx_html ALL
    COMMAND ${SPHINX_BUILD_EXECUTABLE} -M html ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_BINARY_DIR} ${MPRPC_SPHINX_OPTIONS}
    COMMENT "Generate Sphinx html API documentation for mprpc")

add_dependencies(mprpc_doc_sphinx_html mprpc_doc_doxygen)
add_dependencies(mprpc_doc mprpc_doc_sphinx_html)

if(MPRPC_BUILD_LATEX_DOC)
    add_custom_target(
        mprpc_doc_sphinx_latex ALL
        COMMAND
            ${SPHINX_BUILD_EXECUTABLE} -M latexpdf
            ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}
            ${MPRPC_SPHINX_OPTIONS}
        COMMENT "Generate Sphinx latex API documentation for mprpc")

    add_dependencies(mprpc_doc_sphinx_latex mprpc_doc_doxygen)
    add_dependencies(mprpc_doc mprpc_doc_sphinx_latex)

    # prevent parallel build of sphinx documentations
    add_dependencies(mprpc_doc_sphinx_latex mprpc_doc_sphinx_html)
endif()
