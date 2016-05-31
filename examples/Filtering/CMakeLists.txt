project(ITKBenchmarkFiltering)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarks
    ITKIOImageBase
    ITKSmoothing
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
