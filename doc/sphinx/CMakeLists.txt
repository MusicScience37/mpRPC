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
    COMMAND
        ${PIPENV_EXECUTABLE} run sphinx-build -M html
        ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}
        ${MPRPC_SPHINX_OPTIONS}
    COMMENT "Generate Sphinx html API documentation for mprpc"
    WORKING_DIRECTORY ${MPRPC_SOURCE_DIR})

add_dependencies(mprpc_doc mprpc_doc_sphinx_html)
if(MPRPC_PYTHON_INTERFACE)
    add_dependencies(mprpc_doc_sphinx_html mprpc_python_wrapper)
endif()
