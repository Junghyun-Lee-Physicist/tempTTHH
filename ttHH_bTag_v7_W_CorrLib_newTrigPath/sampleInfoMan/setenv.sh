#!/bin/bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")
echo "${SCRIPT_DIR}"

export CPLUS_INCLUDE_PATH="${SCRIPT_DIR}/include:$CPLUS_INCLUDE_PATH"
export LIBRARY_PATH="${SCRIPT_DIR}/lib:$LIBRARY_PATH" # static libary
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:$LD_LIBRARY_PATH" # shared library

echo "CPLUS_INCLUDE_PATH set to: $CPLUS_INCLUDE_PATH"
echo "LIBRARY_PATH set to: $LIBRARY_PATH"
echo "LD_LIBRARY_PATH set to: $LD_LIBRARY_PATH"
