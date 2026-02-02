#!/bin/bash

export CPLUS_INCLUDE_PATH="$(pwd)/../include:$CPLUS_INCLUDE_PATH"
export LIBRARY_PATH="$(pwd)/../lib:$LIBRARY_PATH"
export LD_LIBRARY_PATH="$(pwd)/../lib:$LD_LIBRARY_PATH"

echo "CPLUS_INCLUDE_PATH set to: $CPLUS_INCLUDE_PATH"
echo "LIBRARY_PATH set to: $LIBRARY_PATH"
echo "LD_LIBRARY_PATH set to: $LD_LIBRARY_PATH"
