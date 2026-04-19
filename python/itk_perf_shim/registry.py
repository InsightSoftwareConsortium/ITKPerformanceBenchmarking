"""Maps logical benchmark names to the executable + CLI template.

CLI contract (from examples/*/CMakeLists.txt):
  <exe> <timingsJson> <iterations> <threads> <input...> <output...>

Templates use str.format substitution:
  {timings_json}  — absolute path for the jsonxx output
  {iterations}    — per-benchmark iteration count
  {brain}         — brainweb165a10f17.mha (ExternalData fixture)
  {brain_x45}     — brainweb165a10f17extract45i90z.mha
  {brain_x60}     — brainweb165a10f17extract60i50z.mha
  {output_dir}    — scratch dir for benchmark output images

Scope notes:
  - MorphologicalWatershedBenchmark's CLI omits the threads argument
    (see its argv parsing).
  - Registration and Resample benchmarks are intentionally absent from
    this initial harness; they require ITK-level compiler fixes
    (NrrdIO airFloatQNaN link, ResampleBenchmark -Wmaybe-uninitialized)
    that are outside the scope of this PR.
"""

BENCHMARKS = {
    "core.copy_iteration": {
        "exe": "CopyIterationBenchmark",
        "args": ["{timings_json}", "{iterations}", "128"],
        "iterations": 25,
    },
    "core.vector_iteration": {
        "exe": "VectorIterationBenchmark",
        "args": ["{timings_json}", "{iterations}", "128"],
        "iterations": 50,
    },
    "filtering.binary_add": {
        "exe": "BinaryAddBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "1",
            "{brain}", "{brain}", "{output_dir}/BinaryAddBenchmark.mha",
        ],
        "iterations": 10,
    },
    "filtering.unary_add": {
        "exe": "UnaryAddBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "1",
            "{brain}", "{brain}", "{output_dir}/UnaryAddBenchmark.mha",
        ],
        "iterations": 10,
    },
    "filtering.gradient_magnitude": {
        "exe": "GradientMagnitudeBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "-1",
            "{brain}", "{output_dir}/GradientMagnitudeBenchmark.mha",
        ],
        "iterations": 5,
    },
    "filtering.median": {
        "exe": "MedianBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "-1",
            "{brain}", "{output_dir}/MedianBenchmark.mha",
        ],
        "iterations": 3,
    },
    "filtering.min_max_curvature_flow": {
        "exe": "MinMaxCurvatureFlowBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "-1",
            "{brain}", "{output_dir}/MinMaxCurvatureFlowBenchmark.mha",
        ],
        "iterations": 3,
    },
    "segmentation.region_growing": {
        "exe": "RegionGrowingBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "-1",
            "{brain}", "{output_dir}/RegionGrowingBenchmark.mha",
        ],
        "iterations": 3,
    },
    "segmentation.watershed": {
        "exe": "WatershedBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "-1",
            "{brain_x45}", "{output_dir}/WatershedBenchmark.mha",
        ],
        "iterations": 3,
    },
    "segmentation.morphological_watershed": {
        "exe": "MorphologicalWatershedBenchmark",
        "args": [
            "{timings_json}", "{iterations}",
            "{brain_x45}", "{output_dir}/MorphologicalWatershedBenchmark.mha",
        ],
        "iterations": 3,
    },
    "segmentation.level_set": {
        "exe": "LevelSetBenchmark",
        "args": [
            "{timings_json}", "{iterations}", "-1",
            "{brain_x60}", "{output_dir}/LevelSetBenchmark.mha",
        ],
        "iterations": 3,
    },
}
