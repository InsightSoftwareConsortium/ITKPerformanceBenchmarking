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
  ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__ThreadOverheadBenchmark.json
  1000 )
set_property(TEST ThreadOverheadBenchmark APPEND PROPERTY LABELS Core)
## performance tests should not be run in parallel
set_tests_properties(ThreadOverheadBenchmark PROPERTIES RUN_SERIAL TRUE)

add_executable(VectorIterationBenchmark itkVectorIterationBenchmark.cxx )
target_link_libraries(VectorIterationBenchmark ${ITK_LIBRARIES})
add_test(
  NAME VectorIterationBenchmark
  COMMAND VectorIterationBenchmark
  ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__VectorIterationBenchmark.json
  50 128 )
set_property(TEST VectorIterationBenchmark APPEND PROPERTY LABELS Core)
## performance tests should not be run in parallel
set_tests_properties(VectorIterationBenchmark PROPERTIES RUN_SERIAL TRUE)

add_executable(CopyIterationBenchmark itkCopyIterationBenchmark.cxx )
target_link_libraries(CopyIterationBenchmark ${ITK_LIBRARIES})
add_test(
  NAME CopyIterationBenchmark
  COMMAND CopyIterationBenchmark
  ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__CopyIterationBenchmark.json
  25 128 )
set_property(TEST CopyIterationBenchmark APPEND PROPERTY LABELS Core)
## performance tests should not be run in parallel
set_tests_properties(CopyIterationBenchmark PROPERTIES RUN_SERIAL TRUE)
