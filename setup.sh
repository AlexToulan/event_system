#!/bin/bash

# clean
rm -fr build/
mkdir build

# cmake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j24