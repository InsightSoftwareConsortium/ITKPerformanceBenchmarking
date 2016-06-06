project(ITKBenchmarkSegmentation)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarking
    ITKIOImageBase
    ITKIOMeta
    ITKCurvatureFlow
    ITKRegionGrowing
  )
include(${ITK_USE_FILE})

add_executable(RegionGrowingBenchmark RegionGrowingBenchmark.cxx)
target_link_libraries(RegionGrowingBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME RegionGrowingBenchmark
  COMMAND RegionGrowingBenchmark
    ${TEST_OUTPUT_DIR}/RegionGrowingBenchmark.tsv
    -1
    ${BRAIN_IMAGE}
    ${TEST_OUTPUT_DIR}/RegionGrowingBenchmark.mha
  )
set_property(TEST RegionGrowingBenchmark APPEND PROPERTY LABELS Registration)
