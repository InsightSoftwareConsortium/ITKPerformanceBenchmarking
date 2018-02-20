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
    ${TEST_OUTPUT_DIR}/MedianBenchmark__DATESTAMP__.json
    3
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MedianBenchmark.mha
  )
set_property(TEST MedianBenchmark APPEND PROPERTY LABELS Filtering)

add_executable(GradientMagnitudeBenchmark GradientMagnitudeBenchmark.cxx )
target_link_libraries(GradientMagnitudeBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME GradientMagnitudeBenchmark
  COMMAND GradientMagnitudeBenchmark
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark__DATESTAMP__.json
    16
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark.mha
  )
set_property(TEST GradientMagnitudeBenchmark APPEND PROPERTY LABELS Filtering)

ExternalData_Add_Test(ITKBenchmarksData
  NAME GradientMagnitudeBenchmark1Thread
  COMMAND GradientMagnitudeBenchmark
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark1Thread__DATESTAMP__.json
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
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowBenchmark__DATESTAMP__.json
    3
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowBenchmark.mha
  )
set_property(TEST MinMaxCurvatureFlowBenchmark APPEND PROPERTY LABELS Filtering)

## performance tests should not be run in parallel
set_tests_properties(MedianBenchmark GradientMagnitudeBenchmark MinMaxCurvatureFlowBenchmark PROPERTIES RUN_SERIAL TRUE)
