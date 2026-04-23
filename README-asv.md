# ASV harness for ITKPerformanceBenchmarking

Native ITK C++ benchmark executables driven by
[airspeed velocity (asv)](https://asv.readthedocs.io/) for continuous
performance-regression detection on ITK pull requests.

## Design

- **The C++ benchmarks remain canonical.** ASV does not recompile ITK
  and does not interpret benchmark results numerically.
- **`environment_type: "existing"`** keeps ASV out of the ITK build
  loop. The CI workflow (see `ITK/.github/workflows/perf-benchmark.yml`)
  configures and builds ITK + `ITKPerformanceBenchmarking` twice
  (merge-base, PR HEAD), then invokes `asv run` against each build.
- **`track_*` not `time_*`.** The timing source of truth is
  `HighPriorityRealTimeProbesCollector` inside the C++ subprocess.
  The Python shim reads the jsonxx output and returns mean probe
  seconds. ASV's built-in timing would include subprocess spawn
  overhead, which we want to exclude.

## Environment contract

The shim needs three env vars:

| Var | Purpose |
|-----|---------|
| `ITK_BENCHMARK_BIN` | Dir containing `MedianBenchmark`, `GradientMagnitudeBenchmark`, etc. |
| `ITK_BENCHMARK_DATA` | ExternalData root with `OAS1_0001_MR1_mpr-1_anon.nrrd` fixture |
| `ITK_BENCHMARK_SCRATCH` | Optional scratch dir for per-run output images |

## Local smoke test

```sh
# 1. Build ITK with Module_PerformanceBenchmarking=ON and BUILD_EXAMPLES=ON,
#    then build the 5 benchmark targets (full ITK build is not needed):
cmake -S /path/to/ITK -B /path/to/ITK-build -GNinja -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=ON -DBUILD_EXAMPLES=ON -DBUILD_SHARED_LIBS=OFF \
    -DModule_PerformanceBenchmarking=ON
cmake --build /path/to/ITK-build --target \
    MedianBenchmark BinaryAddBenchmark GradientMagnitudeBenchmark \
    CopyIterationBenchmark VectorIterationBenchmark \
    ITKBenchmarksData

# 2. Create the itk-repo symlink required by asv.conf.json:
cd /path/to/ITKPerformanceBenchmarking
ln -sfn /path/to/ITK itk-repo

# 3. Install asv + the shim into a Python environment:
python -m venv .venv && . .venv/bin/activate
pip install "asv>=0.6,<0.7"
pip install -e python

# 4. Point the shim at the build + data:
export ITK_BENCHMARK_BIN=/path/to/ITK-build/bin
export ITK_BENCHMARK_DATA=/path/to/ITK-build/ExternalData/Modules/Remote/PerformanceBenchmarking/examples/Data/Input
export ITK_BENCHMARK_SCRATCH=/tmp/itkperf

# 5. Register the machine (first run only):
asv machine --yes --machine $(hostname -s)

# 6. Run labelled with the current ITK SHA (environment_type: existing
#    does not accept range specs — --set-commit-hash does the labelling):
ITK_SHA=$(cd itk-repo && git rev-parse HEAD)
asv run --machine $(hostname -s) --set-commit-hash "$ITK_SHA" --python=same
```

## PR comparison pattern (for CI)

```sh
# Assumes ITK-base-build and ITK-head-build already exist, and the
# itk-repo symlink points at the ITK clone.
BASE_SHA=$(cd itk-repo && git merge-base origin/main HEAD)
HEAD_SHA=$(cd itk-repo && git rev-parse HEAD)

ITK_BENCHMARK_BIN=/path/to/ITK-base-build/bin \
    asv run --machine $HOST --set-commit-hash "$BASE_SHA" --python=same

ITK_BENCHMARK_BIN=/path/to/ITK-head-build/bin \
    asv run --machine $HOST --set-commit-hash "$HEAD_SHA" --python=same

asv compare "$BASE_SHA" "$HEAD_SHA" --factor=1.10 --split
asv publish
```

## Status

Prototype — May 2026. Covers 5 benchmarks (Core: 2, Filtering: 3).
Remaining ~10 executables (Registration, Segmentation, Resample
variants) will be added once the end-to-end loop is validated.
