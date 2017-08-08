#!/bin/bash
# This is what is run on the benchmark machine when you push to the repository
mkdir build
cd build; cmake -DCMAKE_BUILD_TYPE=Release ..;make;cd ..
mkdir results
./build/micro_bench/hca_mb > results/hca_mb.txt
