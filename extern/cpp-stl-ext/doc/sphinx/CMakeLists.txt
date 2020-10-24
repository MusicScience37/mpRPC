find_program(
    SPHINX_BUILD_EXECUTABLE
    NAMES sphinx-build
    PATHS ${STL_EXT_SOURCE_DIR}/.venv/bin ${STL_EXT_SOURCE_DIR}/.venv/Scripts
    NO_DEFAULT_PATH)

if(NOT SPHINX_BUILD_EXECUTABLE)
    find_program(SPHINX_BUILD_EXECUTABLE NAMES sphinx-build)
endif()

if(NOT SPHINX_BUILD_EXECUTABLE)
    message("Skipping Sphinx configuration")
    return()
endif()

message("Found Sphinx: ${SPHINX_BUILD_EXECUTABLE}")

option(STL_EXT_BUILD_LATEX_DOC "build latex documentation of stl_ext" OFF)

if(DEFINED ENV{PLANTUML_JAR_PATH})
    set(DEFAULT_PLANTUML_JAR_PATH $ENV{PLANTUML_JAR_PATH})
else()
    set(DEFAULT_PLANTUML_JAR_PATH "")
endif()
set(PLANTUML_JAR_PATH
    "${DEFAULT_PLANTUML_JAR_PATH}"
    CACHE FILEPATH "path of plantuml.jar")

set(STL_EXT_SPHINX_OPTIONS
    -D breathe_projects.stl_ext="${CMAKE_CURRENT_BINARY_DIR}/../doxygen/xml" -D
    "plantuml=java -jar ${PLANTUML_JAR_PATH}" -D release=${PROJECT_VERSION})

add_custom_target(
    stl_ext_doc_sphinx_html ALL
    COMMAND ${SPHINX_BUILD_EXECUTABLE} -M html ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_BINARY_DIR} ${STL_EXT_SPHINX_OPTIONS}
    COMMENT "Generate Sphinx html API documentation for stl_ext")

add_dependencies(stl_ext_doc_sphinx_html stl_ext_doc_doxygen)
add_dependencies(stl_ext_doc stl_ext_doc_sphinx_html)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/start_auto_build.sh.in
               ${CMAKE_CURRENT_BINARY_DIR}/start_auto_build.sh)

if(STL_EXT_BUILD_LATEX_DOC)
    add_custom_target(
        stl_ext_doc_sphinx_latex ALL
        COMMAND
            ${SPHINX_BUILD_EXECUTABLE} -M latexpdf
            ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}
            ${STL_EXT_SPHINX_OPTIONS}
        COMMENT "Generate Sphinx latex API documentation for stl_ext")

    add_dependencies(stl_ext_doc_sphinx_latex stl_ext_doc_doxygen)
    add_dependencies(stl_ext_doc stl_ext_doc_sphinx_latex)

    # prevent parallel build of sphinx documentations
    add_dependencies(stl_ext_doc_sphinx_latex stl_ext_doc_sphinx_html)
endif()
