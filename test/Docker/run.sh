#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker run \
  --rm \
  -v $script_dir/../..:/ITKPerformanceBenchmarking \
    insighttoolkit/performancebenchmarking-test \
      /ITKPerformanceBenchmarking/test/Docker/test.sh
