itk_module_test()

# Add Test code for itkPerformanceBenchmarking
add_executable(itkHighPriorityRealTimeProbeTest
               itkHighPriorityRealTimeProbeTest.cxx )
target_link_libraries(itkHighPriorityRealTimeProbeTest
                      ${ITK_LIBRARIES} PerformanceBenchmarking)
add_test(NAME itkHighPriorityRealTimeProbeTest
         COMMAND itkHighPriorityRealTimeProbeTest )


add_executable(itkTimeProbeTest2
               itkTimeProbeTest2.cxx )
target_link_libraries(itkTimeProbeTest2
                      ${ITK_LIBRARIES} PerformanceBenchmarking)
add_test(NAME itkTimeProbeTest2
         COMMAND itkTimeProbeTest2 )


add_executable(itkHighPriorityRealTimeProbesCollectorTest
               itkHighPriorityRealTimeProbesCollectorTest.cxx )
target_link_libraries(itkHighPriorityRealTimeProbesCollectorTest
                      ${ITK_LIBRARIES} PerformanceBenchmarking)
add_test(NAME itkHighPriorityRealTimeProbesCollectorTest
         COMMAND itkHighPriorityRealTimeProbesCollectorTest )


add_executable(itkTimeProbesTest2
               itkTimeProbesTest2.cxx )
target_link_libraries(itkTimeProbesTest2
                      ${ITK_LIBRARIES} PerformanceBenchmarking)
add_test(NAME itkTimeProbesTest2
         COMMAND itkTimeProbesTest2 )

