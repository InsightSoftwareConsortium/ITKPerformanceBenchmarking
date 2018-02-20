project(ITKBenchmarks NONE)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
include(ITKBenchmarksExternalData)

ExternalData_Expand_Arguments(ITKBenchmarksData
  BRAIN_IMAGE
  "DATA{Data/Input/brainweb165a10f17.mha}"
  )
set(TEST_OUTPUT_DIR "${PROJECT_BINARY_DIR}/Testing")

include(CTest)

option(BENCHMARK_ITK_CORE "Test the performance of ITK Core." ON)
if(BENCHMARK_ITK_CORE)
  add_subdirectory(Core)
endif()

option(BENCHMARK_ITK_FILTERING "Test the performance of ITK Filters." ON)
if(BENCHMARK_ITK_FILTERING)
  add_subdirectory(Filtering)
endif()

option(BENCHMARK_ITK_REGISTRATION "Test the performance of ITK Registration." ON)
if(BENCHMARK_ITK_REGISTRATION)
  add_subdirectory(Registration)
endif()

option(BENCHMARK_ITK_SEGMENTATION "Test the performance of ITK Segmentation." ON)
if(BENCHMARK_ITK_SEGMENTATION)
  add_subdirectory(Segmentation)
endif()

ExternalData_Add_Target(ITKBenchmarksData)
