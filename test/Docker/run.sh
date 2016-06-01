#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker run \
  --rm \
  -v $script_dir/../..:/usr/src/ITKPerformanceBenchmarking \
    insighttoolkit/performancebenchmarking-test \
      /usr/src/ITKPerformanceBenchmarking/test/Docker/test.sh
