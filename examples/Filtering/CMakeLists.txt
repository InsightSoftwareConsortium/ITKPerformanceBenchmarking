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
    ${TEST_OUTPUT_DIR}/MedianBenchmark.tsv
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
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark.tsv
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark.mha
  )
set_property(TEST GradientMagnitudeBenchmark APPEND PROPERTY LABELS Filtering)

ExternalData_Add_Test(ITKBenchmarksData
  NAME GradientMagnitudeBenchmark1Thread
  COMMAND GradientMagnitudeBenchmark
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark1Thread.tsv
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
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowBenchmark.tsv
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowBenchmark.mha
  )
set_property(TEST MinMaxCurvatureFlowBenchmark APPEND PROPERTY LABELS Filtering)
