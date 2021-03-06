find_package(Doxygen REQUIRED dot)

# range of documentation
set(DOXYGEN_EXTRACT_ALL NO)
set(DOXYGEN_EXTRACT_PRIVATE YES)
set(DOXYGEN_EXTRACT_PRIV_VIRTUAL YES)
set(DOXYGEN_EXTRACT_STATIC YES)
set(DOXYGEN_INTERNAL_DOCS YES)

# inputs
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_FILE_PATTERNS *.h *.cpp *.md)
set(DOXYGEN_INPUT_ENCODING UTF-8)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

# warnings
set(DOXYGEN_QUIET YES)
set(DOXYGEN_WARNINGS YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
set(DOXYGEN_WARN_NO_PARAMDOC YES)

# source browsing
set(DOXYGEN_SOURCE_BROWSER YES)

# for class documentations
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_SORT_BRIEF_DOCS YES)
set(DOXYGEN_SORT_MEMBER_DOCS YES)
set(DOXYGEN_SORT_MEMBERS_CTORS_1ST YES)

# for diagrams automatically generated with dot
set(DOXYGEN_DOT_IMAGE_FORMAT svg)
set(DOXYGEN_INTERACTIVE_SVG YES)
set(DOXYGEN_DIRECTORY_GRAPH NO)
set(DOXYGEN_TEMPLATE_RELATIONS YES)
set(DOXYGEN_UML_LOOK YES)
set(DOXYGEN_DOT_NUM_THREADS 1)

# for preprocessors
set(DOXYGEN_ENABLE_PREPROCESSING YES)
set(DOXYGEN_MACRO_EXPANSION YES)
set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
set(DOXYGEN_PREDEFINED STL_EXT_DOCUMENTATION=1 "STL_EXT_UNUSED(PARAM)=PARAM")
set(DOXYGEN_SEARCH_INCLUDES YES)
set(DOXYGEN_INCLUDE_PATH include)
set(DOXYGEN_STRIP_FROM_INC_PATH include)

# for html output
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_HTML_COLORSTYLE_HUE 24)
set(DOXYGEN_HTML_COLORSTYLE_SAT 255)
set(DOXYGEN_HTML_COLORSTYLE_GAMMA 100)
set(DOXYGEN_HTML_TIMESTAMP YES)
set(DOXYGEN_HTML_DYNAMIC_MENUS YES)
set(DOXYGEN_HTML_DYNAMIC_SECTIONS YES)

set(DOXYGEN_GENERATE_XML YES)

# output directory
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(DOXYGEN_PROJECT_NAME "cpp-stl-ext")

doxygen_add_docs(
    stl_ext_doc_doxygen "include" "doc/doxygen" "README.md"
    WORKING_DIRECTORY "${STL_EXT_SOURCE_DIR}"
    ALL
    COMMENT "Generate Doxygen API documentation for ${PROJECT_NAME}")

add_dependencies(stl_ext_doc stl_ext_doc_doxygen)
