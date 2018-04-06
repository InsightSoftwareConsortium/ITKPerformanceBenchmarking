#!/bin/bash
# \author Hans J. Johnson
#
# To properly run, this script requires:
# git   https://git-scm.com/
# CMake https://cmake.org/
# Ninja https://ninja-build.org/
# C and C++ compiler supported by both CMake and Ninja
# bash-compatible shell interpreter, e.g. git bash from https://gitforwindows.org/
#
# If specific compilers are desired, they can be specified via environment variables before invoking this file, e.g.
# export CC="clang"
# export CXX="clang"
# Alternatively, ITK can already be configured in ITK_BLD folder with custom settings (possibly including specific compilers)

startDir=$(pwd) # startDir should be parent folder of this script
echo "startDir: ${startDir}"

if [ $# -eq 0 ] ;then
#  Get a long list of hashes to consider
#  git log v4.12.0..origin/master --reverse --merges --format="%H %ci %aN %s"
#
cd  ${startDir}/ITK
for hash_to_test in $( git log v4.12.0..origin/master --reverse --merges --format="%H" ); do
#   echo "hash_to_test: ${hash_to_test}" &&
   cd  ${startDir} &&
   ${startDir}/ITKPerformanceBenchmarking/ITK_PerformanceTestingDriver.sh \
        --src "${startDir}/ITK" \
        --bld "${startDir}/ITK-bld" \
        --perf-src "${startDir}/ITKPerformanceBenchmarking" \
        --perf-bld "${startDir}/ITKPB-bld" \
        --git-tag ${hash_to_test}
done
exit 0 # Testing finished successfully!
fi


function usage() {
# -s | --src The ITK src directory
# -b | --bld The ITK build directory
# -p | --perf-src The ITKPerformanceBenchmark srcdir
# -t | --perf-bld The ITKPerformanceBenchmark testing (aka build) dir
# -g | --git-tag The git tag to generate performance information for
   echo "Usage: $0 [-s|--src ITK_SRC] [-b|--bld ITK_BLD] [-g|--git-tag GIT_TAG] \\" 1>&2
   echo "          [-p|--perf-src PERF_SRC] [-t|--perf-bld PERF_BLD] [-g|--git-tag GIT_TAG] " 1>&2
}

OPTS=$(getopt -o s:b:p:t:g: --long src:,bld:,perf-src:,perf-bld:,git-tag:  -n '${0}' -- "$@")
if [[ $? -ne 0 ]]; then
   echo "Failed parsing options." 1>&2
   usage
fi

# echo "OPTS: $OPTS"
eval set -- "$OPTS"
while true; do
  case "$1" in
    -s | --src ) ITK_SRC="$2"; shift;shift ;;
    -b | --bld )    ITK_BLD="$2"; shift;shift ;;
    -p | --perf-src ) PERF_SRC="$2"; shift;shift ;;
    -t | --perf-bld ) PERF_BLD="$2"; shift; shift ;;
    -g | --git-tag ) ITK_GIT_TAG="$2"; shift; shift ;;
    -- ) shift; break ;;
    * ) break ;;
  esac
done

echo "ITK_SRC: ${ITK_SRC}"
echo "ITK_BLD: ${ITK_BLD}"
echo "PERF_SRC: ${PERF_SRC}"
echo "PERF_BLD: ${PERF_BLD}"
echo "ITK_GIT_TAG: ${ITK_GIT_TAG}"

## START REAL PROCESSING

#########################
## Get the requested tag
##
pushd ${ITK_SRC}
git stash
git checkout ${ITK_GIT_TAG}  ##     || echo "FAILED TO checkout ITK ${ITK_GIT_TAG}" &&  exit -1
git reset --hard ${ITK_GIT_TAG}
echo "Finished checking out ITK"
popd

#########################
## Generate an environmental variable to supplement report generation
##
pushd ${ITK_SRC}
export GIT_CONFIG_SHA1="$(git rev-parse HEAD)"
export GIT_CONFIG_DATE="$(git show -s --format=%ci HEAD)"
export GIT_LOCAL_MODIFICATIONS="$(git  diff --shortstat HEAD)"
export ITKPERFORMANCEBENCHMARK_AUX_JSON="
{
\"ITK_MANUAL_BUILD_INFO\": {
 \"GIT_CONFIG_DATE\": \"${GIT_CONFIG_DATE}\",
 \"GIT_CONFIG_SHA1\": \"${GIT_CONFIG_SHA1}\",
 \"GIT_LOCAL_MODIFICATIONS\": \"${GIT_LOCAL_MODIFICATIONS}\"
}
}
"
echo "Finished compiling config info."
popd

echo "ITK_SRC: ${ITK_SRC}"
echo "ITK_BLD: ${ITK_BLD}"

#########################
## Build ITK for Performance testing
##
mkdir ${ITK_BLD}
pushd ${ITK_BLD}
echo "Configuring ITK..."
cmake -G Ninja \
      -DCMAKE_BUILD_TYPE:STRING=Release \
      -DCMAKE_CXX_STANDARD:STRING=11 \
      -DBUILD_TESTING:BOOL=OFF \
      -DBUILD_EXAMPLES:BOOL=OFF \
      \
      ${ITK_SRC} > ITK_CONFIG_logger 2>&1
echo "Building ITK..."
ninja > ITK_BUILD_logger 2>&1 # || echo "FAILED TO BUILD ITK: ${ITK_GIT_TAG}" && exit -1
echo "Finished bulding ITK"
popd

#########################
## Build ITKPerformanceBenchmark
##
pushd ${PERF_SRC}
# git checkout origin/master #  || echo "FAILED TO CHECKOUT ITKPerformanceBenchmarking" && exit -1
# echo "Finished checking out ITKPerformanceBenchmark"
popd
echo "Building and testing perf for ${ITK_GIT_TAG} ... \n ${ITKPERFORMANCEBENCHMARK_AUX_JSON}\n"
mkdir -p ${PERF_BLD}
pushd ${PERF_BLD}

# fca883daf05ac62ee0449513dbd2ad30ff9591f0 is sha1 that introduces itk::BuildInformation
# so all ancestors need to prevent the benchmarking from using
pushd ${ITK_SRC}
NO_ITKBUILDINFORMATION=$(git merge-base --is-ancestor HEAD fca883daf05ac62ee0449513dbd2ad30ff9591f0)
popd
if [[ ${NO_ITKBUILDINFORMATION} -eq 0 ]]; then
  ITK_HAS_INFORMATION_H="OFF"
else
  ITK_HAS_INFORMATION_H="ON"
fi

cmake -G Ninja \
      -DCMAKE_BUILD_TYPE:STRING=Release \
      -DCMAKE_CXX_STANDARD:STRING=11 \
      -DITK_DIR:PATH=${ITK_BLD} \
      -DITK_HAS_INFORMATION_H:BOOL=${ITK_HAS_INFORMATION_H} \
      \
      ${PERF_SRC} > PERF_CONFIG_logger 2>&1
ninja > PERF_BUILD_logger 2>&1 && ( pushd ${PERF_BLD}; ctest; popd )
# || echo "FAILED TO BUILD PERFORMANCE BENCHMARKS: ${ITK_GIT_TAG}" && exit -1
echo "Done with performance testing"
popd
