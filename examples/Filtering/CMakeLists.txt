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

add_executable(MedianImageFilterBenchmark MedianImageFilterBenchmark.cxx )
target_link_libraries(MedianImageFilterBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME MedianImageFilterBenchmark
  COMMAND MedianImageFilterBenchmark
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MedianImageFilterBenchmark.mha
    ${TEST_OUTPUT_DIR}/MedianImageFilterBenchmark.tsv
  )
set_property(TEST MedianImageFilterBenchmark APPEND PROPERTY LABELS Filtering)

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
