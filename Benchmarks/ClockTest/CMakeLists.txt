#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Utilities)

# Add Test code for itkTimeProbe
add_executable(itkTimeProbeTest
               itkTimeProbeTest.cxx )
target_link_libraries(itkTimeProbeTest
                      ${ITK_LIBRARIES} )
add_test(NAME itkTimeProbeTest
         COMMAND itkTimeProbeTest )

add_executable(itkTimeProbesTest
               itkTimeProbesTest.cxx )
target_link_libraries(itkTimeProbesTest
                      ${ITK_LIBRARIES} )
add_test(NAME itkTimeProbesTest
         COMMAND itkTimeProbesTest )

# Add Test code for itkTimeProbe
add_executable(itkHighPriorityRealTimeProbeTest
               itkHighPriorityRealTimeProbeTest.cxx )
target_link_libraries(itkHighPriorityRealTimeProbeTest
                      ${ITK_LIBRARIES} ITKBenchmarkUtilities)
add_test(NAME itkHighPriorityRealTimeProbeTest
         COMMAND itkHighPriorityRealTimeProbeTest )

add_executable(itkHighPriorityRealTimeProbesTest
               itkHighPriorityRealTimeProbesTest.cxx )
target_link_libraries(itkHighPriorityRealTimeProbesTest
                      ${ITK_LIBRARIES} ITKBenchmarkUtilities)
add_test(NAME itkHighPriorityRealTimeProbesTest
         COMMAND itkHighPriorityRealTimeProbesTest )
