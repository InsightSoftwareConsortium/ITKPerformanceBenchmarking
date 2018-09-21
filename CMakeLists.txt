cmake_minimum_required(VERSION 3.10.2)

## If building external to ITK, we need to capture
## required build setting from ITK prior to calling
## project()
if(NOT ITK_SOURCE_DIR)
  find_package(ITK REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
  if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11) # Supported values are ``11``, ``14``, and ``17``.
  endif()
  if(NOT CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
  endif()
  if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
  endif()

  foreach(p
      ## Only policies introduced after the cmake_minimum_required
      ## version need to explicitly be set to NEW.
      CMP0070 #3.10.0 Define ``file(GENERATE)`` behavior for relative paths.
      CMP0071 #3.10.0 Let ``AUTOMOC`` and ``AUTOUIC`` process ``GENERATED`` files.
      )
    if(POLICY ${p})
      cmake_policy(SET ${p} NEW)
    endif()
  endforeach()


  # Set a default build type if none was specified
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()
  enable_language(C)
  enable_language(CXX)
endif()

project(PerformanceBenchmarking)

set(PerformanceBenchmarking_LIBRARIES PerformanceBenchmarking)

include_directories(${CMAKE_BINARY_DIR})
if(NOT ITK_SOURCE_DIR)
  include(ITKModuleExternal)
else()
  set(ITK_DIR ${CMAKE_BINARY_DIR})
  itk_module_impl()
endif()
add_subdirectory(examples)
