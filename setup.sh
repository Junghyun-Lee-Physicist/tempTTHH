#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export TNM_PATH=$DIR
export PYTHONPATH=$TNM_PATH/python:$PYTHONPATH
export LD_LIBRARY_PATH=$TNM_PATH/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$TNM_PATH/include:${ROOT_INCLUDE_PATH}

echo "TNM_PATH=$TNM_PATH"

# If we add below line, then you can use the ttHH python modules in your python script
# or set the [ sys.path.append( <path-to-modules-dir> ) ] in your python script
#export PYTHONPATH="$TNM_PATH/python/ttHHmodules:$PYTHONPATH"


