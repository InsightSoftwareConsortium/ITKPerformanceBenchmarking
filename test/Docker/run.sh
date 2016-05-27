#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker run \
  --rm \
  -v $script_dir/../..:/usr/src/ITKPerformanceBenchmarks \
    insighttoolkit/performancebenchmarks-test \
      /usr/src/ITKPerformanceBenchmarks/test/Docker/test.sh
