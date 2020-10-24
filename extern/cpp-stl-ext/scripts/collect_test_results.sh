#!/bin/bash

# usage: collect_test_result.sh <directory in which JUnit XMLs exist>

DIR=$1

python3 $(which junitparser) merge $DIR/stl_ext_test_*.xml $DIR/stl_ext_test_unified.xml
python3 -m junit2htmlreport $DIR/stl_ext_test_unified.xml $DIR/stl_ext_test_unified.html
