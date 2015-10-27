## Setup the option of Test

set(ITK_BENCHMARK_DIRS
    Utilities
    ClockTest
   )

option(BENCHMARK_ITK_FILTER "Test the performance of ITK Filters." ON)
option(BENCHMARK_ITK_REGESTRATION "Test the performance of ITK Registration." ON)
option(BENCHMARK_ITK_SEGMENTATION "Test the performance of ITK Segmentation." ON)

if(ITK_BENCHMARK_FILTER)
  set(ITK_BENCHMARK_DIRS ${ITK_BENCHMARK_DIRS}
      Filtering)
endif()

if(ITK_BENCHMARK_REGESTRATION)
  set(ITK_BENCHMARK_DIRS ${ITK_BENCHMARK_DIRS}
      Registration)
endif()

if(ITK_BENCHMARK_SEGMENTATION)
  set(ITK_BENCHMARK_DIRS ${ITK_BENCHMARK_DIRS}
      Segmentation)
endif()

subdirs(${ITK_BENCHMARK_DIRS})
