project(ITKBenchmarkFiltering)

message("ITKBenchmarkFiltering")

add_executable(Benchmark_FFTDirectInverse Benchmark_FFTDirectInverse.cxx )
target_link_libraries(Benchmark_FFTDirectInverse  ${ITK_LIBRARIES} PerformanceBenchmarks)
