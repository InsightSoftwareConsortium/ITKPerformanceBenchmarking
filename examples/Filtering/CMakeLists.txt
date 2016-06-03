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
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MedianBenchmark.mha
    ${TEST_OUTPUT_DIR}/MedianBenchmark.tsv
  )
set_property(TEST MedianBenchmark APPEND PROPERTY LABELS Filtering)

add_executable(GradientMagnitudeBenchmark GradientMagnitudeBenchmark.cxx )
target_link_libraries(GradientMagnitudeBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME GradientMagnitudeBenchmark
  COMMAND GradientMagnitudeBenchmark
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark.mha
    ${TEST_OUTPUT_DIR}/GradientMagnitudeBenchmark.tsv
  )
set_property(TEST GradientMagnitudeBenchmark APPEND PROPERTY LABELS Filtering)

add_executable(MinMaxCurvatureFlowImageFilterBenchmark MinMaxCurvatureFlowImageFilterBenchmark.cxx )
target_link_libraries(MinMaxCurvatureFlowImageFilterBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME MinMaxCurvatureFlowImageFilterBenchmark
  COMMAND MinMaxCurvatureFlowImageFilterBenchmark
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowImageFilterBenchmark.mha
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowImageFilterBenchmark.tsv
  )
set_property(TEST MinMaxCurvatureFlowImageFilterBenchmark APPEND PROPERTY LABELS Filtering)
