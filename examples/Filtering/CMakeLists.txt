project(ITKBenchmarkFiltering)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarking
    ITKIOImageBase
    ITKIOMeta
    ITKSmoothing
    ITKImageGradient
    ITKCurvatureFlow
  )
include(${ITK_USE_FILE})

add_executable(MedianBenchmark MedianBenchmark.cxx )
target_link_libraries(MedianBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME MedianBenchmark
  COMMAND MedianBenchmark
  ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__MedianBenchmark.json
    3
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MedianBenchmark.mha
  )
set_property(TEST MedianBenchmark APPEND PROPERTY LABELS Filtering)

add_executable(BinaryAddBenchmark BinaryAddBenchmark.cxx )
target_link_libraries(BinaryAddBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME BinaryAddBenchmark
  COMMAND BinaryAddBenchmark
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__BinaryAddBenchmark.json
    50
    1
    ${BRAIN_IMAGE}
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/BinaryAddBenchmark.mha
  )
set_property(TEST BinaryAddBenchmark APPEND PROPERTY LABELS Filtering)

add_executable(UnaryAddBenchmark UnaryAddBenchmark.cxx )
target_link_libraries(UnaryAddBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME UnaryAddBenchmark
  COMMAND UnaryAddBenchmark
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__UnaryAddBenchmark.json
    50
    1
    ${BRAIN_IMAGE}
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/UnaryAddBenchmark.mha
  )
set_property(TEST UnaryAddBenchmark APPEND PROPERTY LABELS Filtering)


add_executable(GradientMagnitudeBenchmark GradientMagnitudeBenchmark.cxx )
target_link_libraries(GradientMagnitudeBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME GradientMagnitudeBenchmark
  COMMAND GradientMagnitudeBenchmark
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__GradientMagnitudeBenchmark.json
    16
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark.mha
  )
set_property(TEST GradientMagnitudeBenchmark APPEND PROPERTY LABELS Filtering)

ExternalData_Add_Test(ITKBenchmarksData
  NAME GradientMagnitudeBenchmark1Thread
  COMMAND GradientMagnitudeBenchmark
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__GradientMagnitudeBenchmark1Thread.json
    3
    1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark1Thread.mha
  )
set_property(TEST GradientMagnitudeBenchmark APPEND PROPERTY LABELS Filtering)

add_executable(MinMaxCurvatureFlowBenchmark MinMaxCurvatureFlowBenchmark.cxx )
target_link_libraries(MinMaxCurvatureFlowBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME MinMaxCurvatureFlowBenchmark
  COMMAND MinMaxCurvatureFlowBenchmark
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__MinMaxCurvatureFlowBenchmark.json
    3
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowBenchmark.mha
  )
set_property(TEST MinMaxCurvatureFlowBenchmark APPEND PROPERTY LABELS Filtering)

add_executable(ResampleBenchmark ResampleBenchmark.cxx)
target_link_libraries(ResampleBenchmark ${ITK_LIBRARIES})

macro(add_resample_benchmark interpolator extrapolator transforms use_composite_transform)
  # Extra parameters variable
  set(extra_parameters "")

  # ITK does not support the 1D Euler or Similarity transforms.
  # Check if transforms has Euler or Similarity, then skip the benchmark for 1D image.
  set(transforms_contains_euler_or_similarity FALSE)
  foreach(transform ${transforms})
    if(${transform} STREQUAL "Euler" OR ${transform} STREQUAL "Similarity")
      set(transforms_contains_euler_or_similarity TRUE)
      break()
    endif()
  endforeach()

  # Check if use_composite_transform is enabled
  if(${use_composite_transform})
    set(extra_parameters ${extra_parameters} "-c")
    set(composite_name Composite)
  endif()

  # Check if extrapolator required by the benchmark
  if(NOT ${extrapolator} STREQUAL "None")
    set(extra_parameters ${extra_parameters} "-e" ${extrapolator})
  endif()

  # Construct the benchmark configuration name
  set(benchmark_config_name "${interpolator}_${extrapolator}_${composite_name}")

  foreach(transform ${transforms})
    set(benchmark_config_name ${benchmark_config_name}${transform})
  endforeach()

  # List of images sizes for Resample benchmarking
  set(benchmark_image_sizes "8192;4096 4096;512 512 256")

  foreach(benchmark_image_size ${benchmark_image_sizes})
    string(REPLACE " " "x" benchmark_image_size_name ${benchmark_image_size})
    string(REPLACE " " ";" benchmark_image_size_list ${benchmark_image_size})

    # Check if input image is 1D and transforms contains Euler or Similarity
    set(skip_benchmark FALSE)
    if(${benchmark_image_size} EQUAL "8192" AND ${transforms_contains_euler_or_similarity})
      set(skip_benchmark TRUE)
    endif()

    # Construct the benchmark name
    set(benchmark_command "ResampleBenchmark")
    set(benchmark_name "${benchmark_command}_${benchmark_config_name}_${benchmark_image_size_name}")

    # Add the benchmark
    if(NOT ${skip_benchmark})
      add_test(
        NAME ${benchmark_name}
        COMMAND ${benchmark_command}
        -tf ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__${benchmark_name}.json
        -is ${benchmark_image_size_list}
        -i ${interpolator}
        -t ${transforms}
        -out ${TEST_OUTPUT_DIR}/${benchmark_name}.mha
        -iterations 1
        ${extra_parameters}
        )

      # Set the benchmark properties
      set_property(TEST ${benchmark_name} APPEND PROPERTY LABELS Filtering)
      set_tests_properties(${benchmark_name} PROPERTIES RUN_SERIAL true)
    endif()
  endforeach()
endmacro()

# Resample with Affine transform
add_resample_benchmark(Nearest None Affine FALSE)
add_resample_benchmark(Linear None Affine FALSE)
add_resample_benchmark(BSpline None Affine FALSE)

# Resample with BSpline transform
add_resample_benchmark(Nearest None BSpline FALSE)
add_resample_benchmark(Linear None BSpline FALSE)
add_resample_benchmark(BSpline None BSpline FALSE)

# Resample with Affine transform in composite transform
add_resample_benchmark(Nearest None Affine TRUE)
add_resample_benchmark(Linear None Affine TRUE)
add_resample_benchmark(BSpline None Affine TRUE)

# Resample with BSpline transform in composite transform
add_resample_benchmark(Nearest None BSpline TRUE)
add_resample_benchmark(Linear None BSpline TRUE)
add_resample_benchmark(BSpline None BSpline TRUE)

# Resample with multiple transforms Affine and BSpline
add_resample_benchmark(Nearest None "Affine;BSpline" TRUE)
add_resample_benchmark(Linear None "Affine;BSpline" TRUE)
add_resample_benchmark(BSpline None "Affine;BSpline" TRUE)

# Resample with all transforms and extrapolator
add_resample_benchmark(Nearest Nearest "Translation;Affine;BSpline;Euler;Similarity" TRUE)
add_resample_benchmark(Linear Nearest "Translation;Affine;BSpline;Euler;Similarity" TRUE)
add_resample_benchmark(BSpline Nearest "Translation;Affine;BSpline;Euler;Similarity" TRUE)

# Resample with multiple BSplines and extrapolator
add_resample_benchmark(Nearest Nearest "Translation;Affine;BSpline;BSpline;BSpline" TRUE)
add_resample_benchmark(Linear Nearest "Translation;Affine;BSpline;BSpline;BSpline" TRUE)
add_resample_benchmark(BSpline Nearest "Translation;Affine;BSpline;BSpline;BSpline" TRUE)

# performance tests should not be run in parallel
set_tests_properties(MedianBenchmark BinaryAddBenchmark UnaryAddBenchmark GradientMagnitudeBenchmark MinMaxCurvatureFlowBenchmark PROPERTIES RUN_SERIAL TRUE)
