# An option to allow building against ITK before 2018-02-20 when itkBuildInformation.h was not available
if (ITK_VERSION_MAJOR LESS 5)
  set(HAS_INFO OFF)
else()
  set(HAS_INFO ON)
endif()
option(ITK_HAS_INFORMATION_H "If itk::BuildInformation::GetInstance is available (after 2018-02-20)" ${HAS_INFO})

include(${CMAKE_CURRENT_LIST_DIR}/../CMake/GetGitRevisionDescription.cmake)
get_git_head_revision(GIT_CONFIG_REFSPEC GIT_CONFIG_SHA1)
git_describe(GIT_VERSION_DESC)
git_head_date(GIT_CONFIG_DATE)
git_head_localmods(GIT_LOCAL_MODIFICATIONS)
if( GIT_LOCAL_MODIFICATIONS MATCHES ".*files changed.*")
   set(GIT_LOCAL_MODIFICATIONS " (with uncommitted code modifications ${GIT_LOCAL_MODIFICATIONS} )")
endif()

string(TIMESTAMP CONFIGURE_DATE "%Y-%m-%d %H:%M")

## MAKE_MAP_ENTRY is a macro to facilitate placing items in the itk::PerformanceBenchmarkingInformation class
##                /--------------------------_---------------------------_-------------------------------------------------/
##                |    KEY                   |    VALUE                  | Description of value                            |
##                \--------------------------\---------------------------\-------------------------------------------------\
set(MAPPING_VALUES "
  MAKE_MAP_ENTRY( \"CONFIGURE_DATE\",        \"${CONFIGURE_DATE}\",       \"The date of configuration.\"              );
  MAKE_MAP_ENTRY( \"PROJECT_URL\",           \"https://github.com/InsightSoftwareConsortium/ITKPerformanceBenchmarking\",
                                                                                      \"The URL of project.\"              );

  MAKE_MAP_ENTRY( \"GIT_CONFIG_SHA1\",       \"${GIT_CONFIG_SHA1}${GIT_LOCAL_MODIFICATIONS}\",
                                                                               \"The git HEAD SHA1 has at configuration.\" );
  MAKE_MAP_ENTRY( \"GIT_CONFIG_REFSPEC\",    \"${GIT_CONFIG_REFSPEC}\",   \"The git HEAD ref spec.\"                  );
  MAKE_MAP_ENTRY( \"GIT_CONFIG_DATE\",       \"${GIT_CONFIG_DATE}\",      \"The git HEAD date\"                       );
  MAKE_MAP_ENTRY( \"GIT_VERSION_DESC\",      \"${GIT_VERSION_DESC}\",     \"The git HEAD version tag-commits-hash\"   );

  MAKE_MAP_ENTRY( \"CMAKE_VERSION\",         \"${CMAKE_VERSION}\",        \"The version of cmake used to configure.\" );
  MAKE_MAP_ENTRY( \"CMAKE_GENERATOR\",       \"${CMAKE_GENERATOR}\",      \"The generator used for building.\"        );
  MAKE_MAP_ENTRY( \"CMAKE_BUILD_TYPE\",      \"${CMAKE_BUILD_TYPE}\",     \"The build type requested.\"               );

  MAKE_MAP_ENTRY( \"CMAKE_C_COMPILER_ID\",   \"${CMAKE_C_COMPILER_ID}\",  \"The C compiler ID used for building.\"    );
  MAKE_MAP_ENTRY( \"CMAKE_C_COMPILER_VERSION\",   \"${CMAKE_C_COMPILER_VERSION}\",
                                                                          \"The C compiler version used for building.\"    );
  MAKE_MAP_ENTRY( \"CMAKE_C_COMPILER\",      \"${CMAKE_C_COMPILER}\",     \"The C compiler.\"                       );
  MAKE_MAP_ENTRY( \"CMAKE_C_FLAGS\",         \"${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}\",
                                                                          \"The C compiler flags.\"                 );

  MAKE_MAP_ENTRY( \"CMAKE_CXX_STANDARD\",    \"${CMAKE_CXX_STANDARD}\",   \"The CXX standard specified.\"             );
  MAKE_MAP_ENTRY( \"CMAKE_CXX_COMPILER_ID\", \"${CMAKE_CXX_COMPILER_ID}\",\"The CXX compiler ID used for building.\"  );
  MAKE_MAP_ENTRY( \"CMAKE_CXX_COMPILER_VERSION\",   \"${CMAKE_CXX_COMPILER_VERSION}\",
                                                                          \"The CXX compiler version used for building.\"  );
  MAKE_MAP_ENTRY( \"CMAKE_CXX_COMPILER\",    \"${CMAKE_CXX_COMPILER}\",   \"The CXX compiler.\"                       );
  MAKE_MAP_ENTRY( \"CMAKE_CXX_FLAGS\",       \"${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}\",
                                                                          \"The CXX compiler flags.\"                 );
")

configure_file(PerformanceBenchmarkingInformation.cxx.in ${CMAKE_BINARY_DIR}/PerformanceBenchmarkingInformation.cxx)
configure_file(PerformanceBenchmarkingInformation.h.in ${CMAKE_BINARY_DIR}/include/PerformanceBenchmarkingInformation.h)

set( PerformanceBenchmarking_SRCS
    jsonxx.cc ## MIT License https://github.com/hjiang/jsonxx
    ${CMAKE_BINARY_DIR}/PerformanceBenchmarkingInformation.cxx
    itkHighPriorityRealTimeClock.cxx
    itkHighPriorityRealTimeProbe.cxx
    itkHighPriorityRealTimeProbesCollector.cxx
    PerformanceBenchmarkingUtilities.cxx
    ${CMAKE_BINARY_DIR}/include/PerformanceBenchmarkingInformation.h)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
itk_module_add_library(PerformanceBenchmarking ${PerformanceBenchmarking_SRCS})
