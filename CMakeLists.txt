cmake_minimum_required(VERSION 3.22.1)

# Need to set CMAKE_CXX_STANDARD, CMAKE_CXX_STANDARD_REQUIRED, CMAKE_CXX_EXTENSIONS
# before calling project to ensure that it is available globally within your CMake project.
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17) # Supported values are ``14``, ``17`` and ``20``.
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(NOT CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()


project(PerformanceBenchmarking
  VERSION 0.9.0
  DESCRIPTION
    "Real-world tests to benchmark ITK performance."
  HOMEPAGE_URL "https://github.com/InsightSoftwareConsortium/ITKPerformanceBenchmarking"
  LANGUAGES CXX C
)
if(NOT ITK_SOURCE_DIR)
  find_package(ITK REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})

  foreach(p
      ## Only policies introduced after the cmake_minimum_required
      ## version need to explicitly be set to NEW.
      # currently empty
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
endif()
set(PerformanceBenchmarking_LIBRARIES PerformanceBenchmarking)

include_directories(${CMAKE_BINARY_DIR})
if(NOT ITK_SOURCE_DIR)
  include(ITKModuleExternal)
else()
  set(ITK_DIR ${CMAKE_BINARY_DIR})
  itk_module_impl()
endif()
add_subdirectory(examples)
