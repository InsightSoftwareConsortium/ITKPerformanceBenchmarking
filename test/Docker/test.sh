#!/bin/bash

# This is a script to build the modules and run the test suite in the base
# Docker container.

set -x
set -o

cd /usr/src/ITKPerformanceBenchmarking
branch=$(git rev-parse --abbrev-ref HEAD)
date=$(date +%F_%H_%M_%S)
sha=$(git rev-parse --short HEAD)

cd /usr/src/ITKPerformanceBenchmarking-build
cmake \
  -G Ninja \
  -DITK_DIR:PATH=/usr/src/ITK-build \
  -DCMAKE_BUILD_TYPE:STRING=Release \
  -DBUILDNAME:STRING=External-PerformanceBenchmarking-${branch}-${date}-${sha} \
    /usr/src/ITKPerformanceBenchmarking
ctest -VV -D Experimental

cd /usr/src/ITKBenchmarks-build
cmake \
  -G Ninja \
  -DITK_DIR:PATH=/usr/src/ITK-build \
  -DCMAKE_BUILD_TYPE:STRING=Release \
    /usr/src/ITKPerformanceBenchmarking/examples/
ninja
ctest -V
