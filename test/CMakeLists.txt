itk_module_test()

set(PerformanceBenchmarkingTests_SRCS
  itkHighPriorityRealTimeProbesCollectorTest.cxx
  itkHighPriorityRealTimeProbeTest.cxx
  itkTimeProbeTest2.cxx
  itkTimeProbesTest2.cxx
  )

CreateTestDriver(PerformanceBenchmarking "${PerformanceBenchmarking-Test_LIBRARIES}" "${PerformanceBenchmarkingTests_SRCS}")

itk_add_test(NAME itkHighPriorityRealTimeProbeTest
  COMMAND PerformanceBenchmarkingTestDriver
    itkHighPriorityRealTimeProbeTest
  )

itk_add_test(NAME itkTimeProbeTest2
  COMMAND PerformanceBenchmarkingTestDriver
    itkTimeProbeTest2
  )

itk_add_test(NAME itkHighPriorityRealTimeProbesCollectorTest
  COMMAND PerformanceBenchmarkingTestDriver
    itkHighPriorityRealTimeProbesCollectorTest
  )

itk_add_test(NAME itkTimeProbesTest2
  COMMAND PerformanceBenchmarkingTestDriver
    itkTimeProbesTest2
  )
