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
curl -L https://raw.githubusercontent.com/InsightSoftwareConsortium/ITK/dashboard/itk_common.cmake -O
cat > dashboard.cmake << EOF
set(CTEST_SITE "ITKPerformanceBenchmarkingDockerContainer")
set(CTEST_BUILD_NAME "External-PerformanceBenchmarking-${branch}-${date}-${sha}")
set(CTEST_BUILD_CONFIGURATION "Release")
set(CTEST_CMAKE_GENERATOR "Ninja")
set(CTEST_BUILD_FLAGS: "-j5")
set(CTEST_SOURCE_DIRECTORY /ITKPerformanceBenchmarking)
set(CTEST_BINARY_DIRECTORY /ITKPerformanceBenchmarking-build)
set(dashboard_model Experimental)
set(dashboard_no_clean 1)
set(dashboard_cache "
ITK_DIR:PATH=/ITK-build
BUILD_TESTING:BOOL=ON
")
include(\${CTEST_SCRIPT_DIRECTORY}/itk_common.cmake)
EOF
ctest -VV -S dashboard.cmake

cd /ITKBenchmarks-build
curl -L https://raw.githubusercontent.com/InsightSoftwareConsortium/ITK/dashboard/itk_common.cmake -O
cat > dashboard.cmake << EOF
set(CTEST_SITE "ITKPerformanceBenchmarkingDockerContainer")
set(CTEST_BUILD_NAME "External-PerformanceBenchmarking-Benchmarks-${branch}-${date}-${sha}")
set(CTEST_BUILD_CONFIGURATION "Release")
set(CTEST_CMAKE_GENERATOR "Ninja")
set(CTEST_BUILD_FLAGS: "-j5")
set(CTEST_SOURCE_DIRECTORY /ITKPerformanceBenchmarking/examples)
set(CTEST_BINARY_DIRECTORY /ITKBenchmarks-build)
set(dashboard_model Experimental)
set(dashboard_no_clean 1)
set(dashboard_cache "
ITK_DIR:PATH=/ITK-build
")
include(\${CTEST_SCRIPT_DIRECTORY}/itk_common.cmake)
EOF
ctest -VV -S dashboard.cmake
