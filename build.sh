#!/bin/bash
cd "$(dirname "$0")"
mkdir build 
cd build
cmake ..
make