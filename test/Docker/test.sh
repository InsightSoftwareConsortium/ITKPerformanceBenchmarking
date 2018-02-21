#!/bin/bash

# This is a script to build the modules and run the test suite in the base
# Docker container.

set -x
set -o

cd /ITKPerformanceBenchmarking
branch=$(git rev-parse --abbrev-ref HEAD)
date=$(date +%F_%H_%M_%S)
sha=$(git rev-parse --short HEAD)

cd /ITKPerformanceBenchmarking-build
cmake \
  -G Ninja \
  -DITK_DIR:PATH=/ITK-build \
  -DCMAKE_BUILD_TYPE:STRING=Release \
  -DBUILDNAME:STRING=External-PerformanceBenchmarking-${branch}-${date}-${sha} \
    /ITKPerformanceBenchmarking
ctest -VV -D Experimental

cd /ITKBenchmarks-build
cmake \
  -G Ninja \
  -DITK_DIR:PATH=/ITK-build \
  -DCMAKE_BUILD_TYPE:STRING=Release \
    /ITKPerformanceBenchmarking/examples/
ninja
ctest -V
