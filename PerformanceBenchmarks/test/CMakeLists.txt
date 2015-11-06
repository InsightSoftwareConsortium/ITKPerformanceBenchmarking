itk_module_test()

#set(ITKGPUCommon-tests
# itkGPUImageTest.cxx
# itkGPUReductionTest.cxx
# )

#CreateTestDriver(ITKGPUCommon "${ITKGPUCommon-Test_LIBRARIES}" "${ITKGPUCommon-tests}")
#itk_add_test(NAME itkGPUImageTest COMMAND ITKGPUCommonTestDriver itkGPUImageTest)
#itk_add_test(NAME itkGPUReductionTest COMMAND ITKGPUCommonTestDriver itkGPUReductionTest)

# Add Test code for itkTimeProbe
add_executable(itkHighPriorityRealTimeProbeTest
               itkHighPriorityRealTimeProbeTest.cxx )
target_link_libraries(itkHighPriorityRealTimeProbeTest
                      ${ITK_LIBRARIES} PerformanceBenchmarks)
add_test(NAME itkHighPriorityRealTimeProbeTest
         COMMAND itkHighPriorityRealTimeProbeTest )


add_executable(itkHighPriorityRealTimeProbesCollectorTest3
               itkHighPriorityRealTimeProbesCollectorTest3.cxx )
target_link_libraries(itkHighPriorityRealTimeProbesCollectorTest3
                      ${ITK_LIBRARIES} PerformanceBenchmarks)
add_test(NAME itkHighPriorityRealTimeProbesCollectorTest3
         COMMAND itkHighPriorityRealTimeProbesCollectorTest3 )
