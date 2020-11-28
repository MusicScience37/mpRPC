# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import textwrap

# -- Project information -----------------------------------------------------

project = 'mpRPC'
release = 'unknown'  # inserted in command line

copyright = '2020, MusicScience37 (Kenta Kabashima)'
author = 'MusicScience37'


# -- General configuration ---------------------------------------------------

extensions = [
    'sphinx.ext.todo',
]

templates_path = ['_templates']

exclude_patterns = []

# -- Options for autodoc -----------------------------------------------------

extensions += ['sphinx.ext.autodoc', 'sphinx.ext.napoleon']

autodoc_default_options = {
    'members': None,
    'member-order': 'bysource',
    'inherited-members': None,
}

# -- Options for breathe -----------------------------------------------------

extensions += ['breathe']

# this variable must be set from command line
breathe_projects = {'mprpc': ''}

breathe_default_project = 'mprpc'

breathe_default_members = ('members')

breathe_domain_by_extension = {
    "h": "cpp",
}

# -- Options for exhale ------------------------------------------------------

extensions += ['exhale']
exhale_args = {
    "containmentFolder": "./cpp-api",
    "rootFileName": "index.rst",
    "rootFileTitle": "C++ API Reference",
    "doxygenStripFromPath":  "../../../include",
    "createTreeView": True,
    "exhaleExecutesDoxygen": True,
    "exhaleDoxygenStdin": textwrap.dedent('''
        INPUT = ../../../include
        RECURSIVE = YES
        FILE_PATTERNS = *.h *.cpp
        BUILTIN_STL_SUPPORT = YES
        ENABLE_PREPROCESSING = YES
        MACRO_EXPANSION = YES
        EXPAND_ONLY_PREDEF = YES
        PREDEFINED += MPRPC_DOCUMENTATION=1 MPRPC_EXPORT=""
        SEARCH_INCLUDES = YES
        INCLUDE_PATH = ../../../include
        XML_OUTPUT = .
    '''),
}

# -- Options for PlantUML ----------------------------------------------------

extensions += ['sphinxcontrib.plantuml']

plantuml_output_format = 'svg'

plantuml_syntax_error_image = True

# -- Options for HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'

html_static_path = ['_static']

# -- Options for latex output ------------------------------------------------

latex_documents = [(
    # start doc name
    'index',
    # target name
    'mprpc.tex',
    # title
    'mpRPC',
    # author
    'MusicScience37',
    # document class
    'manual',
    # toc tree only
    True,
)]

latex_elements = {
    'fontpkg': r'''
    \usepackage{lmodern}
    ''',
}
