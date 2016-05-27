project(ITKBenchmarkFiltering)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarks
    ITKFFT
    ITKIOImageBase
    ITKImageGrid
  )
include(${ITK_USE_FILE})

add_executable(Benchmark_FFTDirectInverse Benchmark_FFTDirectInverse.cxx )
target_link_libraries(Benchmark_FFTDirectInverse  ${ITK_LIBRARIES} PerformanceBenchmarks)
