#!/bin/bash

# usage: collect_test_result.sh <directory in which JUnit XMLs exist>

DIR=$1

python3 $(which junitparser) merge $DIR/mprpc_test_*.xml $DIR/mprpc_test_unified.xml
python3 -m junit2htmlreport $DIR/mprpc_test_unified.xml $DIR/mprpc_test_unified.html
