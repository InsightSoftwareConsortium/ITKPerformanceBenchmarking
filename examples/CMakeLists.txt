cmake_minimum_required(VERSION 3.9.5)

project(ITKBenchmarks NONE)
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
enable_language(C)
enable_language(CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

if (NOT DEFINED ITKBenchmarks_SUPERBUILD)
  option(ITKBenchmarks_SUPERBUILD "Build ITK and the PerformanceBenchmarking module prior to building the benchmarks." ON)
endif()


if(ITKBenchmarks_SUPERBUILD AND NOT ITK_DIR)
  # External Project Build of ITK
  include(ExternalProject)
  set(ITKBenchmarks_EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}/bin")

  if(NOT ITK_DIR)
    set(ITK_SOURCE_DIR ${CMAKE_BINARY_DIR}/ITK-src)
    set(ITK_BUILD_DIR ${CMAKE_BINARY_DIR}/ITK-build CACHE INTERNAL "Path to store itk binaries")
    set(ITK_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ITK-install CACHE INTERNAL "Path to store itk install")

    # ITK master 2017-07-03
    ExternalProject_Add(ITK
      GIT_REPOSITORY "https://itk.org/ITK.git"
      GIT_TAG 07cb23f9866768b5f4ee48ebec8766b6e19efc69
      UPDATE_COMMAND ""
      DOWNLOAD_DIR ${ITK_SOURCE_DIR}
      SOURCE_DIR ${ITK_SOURCE_DIR}
      BINARY_DIR ${ITK_BUILD_DIR}
      INSTALL_DIR ${ITK_INSTALL_DIR}
      CMAKE_GENERATOR ${CMAKE_GENERATOR}
      CMAKE_ARGS
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${ITKBenchmarks_EXECUTABLE_OUTPUT_PATH}
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_CXX_COMPILER_ARG1:STRING=${CMAKE_CXX_COMPILER_ARG1}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
        -DCMAKE_C_COMPILER_ARG1:STRING=${CMAKE_C_COMPILER_ARG1}
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DITK_BUILD_DEFAULT_MODULES:BOOL=OFF
        -DModule_ITKTestKernel:BOOL=ON
        -DModule_ITKIOImageBase:BOOL=ON
        -DModule_ITKIOMeta:BOOL=ON
        -DModule_ITKIONRRD:BOOL=ON
        -DModule_ITKIOTransformBase:BOOL=ON
        -DModule_ITKIOTransformHDF5:BOOL=ON
        -DModule_ITKSmoothing:BOOL=ON
        -DModule_ITKImageGradient:BOOL=ON
        -DModule_ITKCurvatureFlow:BOOL=ON
        -DModule_ITKRegistrationMethodsv4:BOOL=ON
        -DModule_ITKMetricsv4:BOOL=ON
        -DModule_ITKPDEDeformableRegistration:BOOL=ON
        -DModule_ITKConvolution:BOOL=ON
        -DModule_ITKRegionGrowing:BOOL=ON
        -DModule_ITKWatersheds:BOOL=ON
        -DModule_ITKConnectedComponents:BOOL=ON
        -DModule_ITKAnisotropicSmoothing:BOOL=ON
        -DModule_ITKFastMarching:BOOL=ON
        -DModule_ITKLevelSets:BOOL=ON
        -DBUILD_TESTING:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
      INSTALL_COMMAND ""
      )
    set(ITK_DIR ${ITK_BUILD_DIR})

    set(PerformanceBenchmarking_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../" CACHE INTERNAL "Path to PerformanceBenchmarking")

    ExternalProject_Add(PerformanceBenchmarkingModule
      UPDATE_COMMAND ""
      DOWNLOAD_COMMAND ""
      SOURCE_DIR "${PerformanceBenchmarking_SOURCE_DIR}"
      BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/PerformanceBenchmarkingModule-build"
      CMAKE_GENERATOR ${CMAKE_GENERATOR}
      CMAKE_ARGS
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${ITKBenchmarks_EXECUTABLE_OUTPUT_PATH}
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_CXX_COMPILER_ARG1:STRING=${CMAKE_CXX_COMPILER_ARG1}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
        -DCMAKE_C_COMPILER_ARG1:STRING=${CMAKE_C_COMPILER_ARG1}
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DITK_DIR:PATH=${ITK_DIR}
      INSTALL_COMMAND ""
      DEPENDS ITK
      )
  endif()

  ExternalProject_Add(ITKBenchmarks
    UPDATE_COMMAND ""
    DOWNLOAD_COMMAND ""
    SOURCE_DIR "${PerformanceBenchmarking_SOURCE_DIR}/examples"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/ITKBenchmarks-build"
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_ARGS
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${ITKBenchmarks_EXECUTABLE_OUTPUT_PATH}
      -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_COMPILER_ARG1:STRING=${CMAKE_CXX_COMPILER_ARG1}
      -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
      -DCMAKE_C_COMPILER_ARG1:STRING=${CMAKE_C_COMPILER_ARG1}
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
      -DITK_DIR:PATH=${ITK_DIR}
      -DITKBenchmarks_SUPERBUILD:BOOL=OFF
    INSTALL_COMMAND ""
    DEPENDS PerformanceBenchmarkingModule
    )
  return()
endif()

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
