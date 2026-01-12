#!/bin/bash

#g++ -I../include main.cpp -L../lib -lSampleInfoManager -o test_exe

source "setenv.sh"
g++ main.cpp -lSampleInfoManager -o test_exe

echo -e "\nNow let's read the sample info csv using SampleInfoManager library\n"
echo "-------------------------------------"
./test_exe
echo -e "-------------------------------------\n"

rm -rf test_exe
