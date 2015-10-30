#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Utilities)

# Add Test code for itkTimeProbe
add_executable(itkTimeProbeTest
               itkTimeProbeTest.cxx )
target_link_libraries(itkTimeProbeTest
                      ${ITK_LIBRARIES} )
add_test(NAME itkTimeProbeTest
         COMMAND itkTimeProbeTest )

add_executable(itkTimeProbesCollectorTest
               itkTimeProbesCollectorTest.cxx )
target_link_libraries(itkTimeProbesCollectorTest
                      ${ITK_LIBRARIES} )
add_test(NAME itkTimeProbesCollectorTest
         COMMAND itkTimeProbesCollectorTest )

# Add Test code for itkTimeProbe
add_executable(itkHighPriorityRealTimeProbeTest
               itkHighPriorityRealTimeProbeTest.cxx )
target_link_libraries(itkHighPriorityRealTimeProbeTest
                      ${ITK_LIBRARIES} ITKBenchmarkUtilities)
add_test(NAME itkHighPriorityRealTimeProbeTest
         COMMAND itkHighPriorityRealTimeProbeTest )

add_executable(itkHighPriorityRealTimeProbeTest2
               itkHighPriorityRealTimeProbeTest2.cxx )
target_link_libraries(itkHighPriorityRealTimeProbeTest2
                      ${ITK_LIBRARIES} ITKBenchmarkUtilities)
add_test(NAME itkHighPriorityRealTimeProbeTest2
         COMMAND itkHighPriorityRealTimeProbeTest2 )

add_executable(itkHighPriorityRealTimeProbesCollectorTest
               itkHighPriorityRealTimeProbesCollectorTest.cxx )
target_link_libraries(itkHighPriorityRealTimeProbesCollectorTest
                      ${ITK_LIBRARIES} ITKBenchmarkUtilities)
add_test(NAME itkHighPriorityRealTimeProbesCollectorTest
         COMMAND itkHighPriorityRealTimeProbesCollectorTest )
