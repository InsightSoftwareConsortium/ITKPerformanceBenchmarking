project(ITKBenchmarkFiltering)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarking
    ITKIOImageBase
    ITKSmoothing
    ITKImageGradient
    ITKCurvatureFlow
    ITKIOMeta
  )
include(${ITK_USE_FILE})

add_executable(MedianImageFilterBenchmark MedianImageFilterBenchmark.cxx )
target_link_libraries(MedianImageFilterBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarkData
  NAME MedianImageFilterBenchmarkTest
  COMMAND MedianImageFilterBenchmark
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MedianImageFilterBenchmark.mha
    ${TEST_OUTPUT_DIR}/MedianImageFilterBenchmark.tsv
  )
set_property(TEST MedianImageFilterBenchmarkTest APPEND PROPERTY LABELS Filtering)

add_executable(GradientMagnitudeRecursiveGaussianImageFilterBenchmark GradientMagnitudeRecursiveGaussianImageFilterBenchmark.cxx )
target_link_libraries(GradientMagnitudeRecursiveGaussianImageFilterBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarkData
  NAME GradientMagnitudeRecursiveGaussianImageFilterBenchmarkTest
  COMMAND GradientMagnitudeRecursiveGaussianImageFilterBenchmark
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/GradientMagnitudeRecursiveGaussianImageFilterBenchmark.mha
    ${TEST_OUTPUT_DIR}/GradientMagnitudeRecursiveGaussianImageFilterBenchmark.tsv
  )
set_property(TEST GradientMagnitudeRecursiveGaussianImageFilterBenchmarkTest APPEND PROPERTY LABELS Filtering)

add_executable(MinMaxCurvatureFlowImageFilterBenchmark MinMaxCurvatureFlowImageFilterBenchmark.cxx )
target_link_libraries(MinMaxCurvatureFlowImageFilterBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarkData
  NAME MinMaxCurvatureFlowImageFilterBenchmarkTest
  COMMAND MinMaxCurvatureFlowImageFilterBenchmark
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowImageFilterBenchmark.mha
    ${TEST_OUTPUT_DIR}/MinMaxCurvatureFlowImageFilterBenchmark.tsv
  )
set_property(TEST MinMaxCurvatureFlowImageFilterBenchmarkTest APPEND PROPERTY LABELS Filtering)
