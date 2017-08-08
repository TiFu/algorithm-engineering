#!/bin/bash
# This is what is run on the benchmark machine when you push to the repository
mkdir build
cd build;
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 8
cd ..
mkdir results
cd build/micro_benchs
./hca_mb > ../../results/hca_mb.txt
