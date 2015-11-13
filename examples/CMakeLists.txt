cmake_minimum_required(VERSION 2.8)
project(ITK_BENCHMARK)

message("Example Directory")

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(ITK_TEST_DRIVER itkTestDriver)

option(BENCHMARK_ITK_FILTER "Test the performance of ITK Filters." ON)
option(BENCHMARK_ITK_REGESTRATION "Test the performance of ITK Registration." ON)
option(BENCHMARK_ITK_SEGMENTATION "Test the performance of ITK Segmentation." ON)

# Add ITK Benchmark for Filtering
if(ITK_BENCHMARK_FILTER)
  add_subdirectory(Filtering)
endif()

# Add ITK Benchmark for Registration
if(ITK_BENCHMARK_REGESTRATION)
  add_subdirectory(Registration)
endif()

# Add ITK Benchmark for Segmentation
if(ITK_BENCHMARK_SEGMENTATION)
  add_subdirectory(Segmentation)
endif()
