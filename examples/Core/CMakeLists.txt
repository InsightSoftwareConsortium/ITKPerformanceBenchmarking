project(ITKBenchmarkCore)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarking
    ITKCommon
  )
include(${ITK_USE_FILE})


add_executable(ThreadOverheadBenchmark ThreadOverhead.cxx )
target_link_libraries(ThreadOverheadBenchmark ${ITK_LIBRARIES})
add_test(
  NAME ThreadOverheadBenchmark
  COMMAND ThreadOverheadBenchmark
  ${TEST_OUTPUT_DIR}/ThreadOverheadBenchmark.tsv
  1000 )
set_property(TEST ThreadOverheadBenchmark APPEND PROPERTY LABELS Core)
