#!/bin/bash

echo "No warning compiling..."
sleep 1

make -j 2>&1 | grep -i "error:"
