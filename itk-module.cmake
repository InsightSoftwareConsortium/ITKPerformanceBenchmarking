set(DOCUMENTATION "ITK Performance Benchmarks")

itk_module(PerformanceBenchmarking
  ENABLE_SHARED
  PRIVATE_DEPENDS
    ITKCommon
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
  "${DOCUMENTATION}"
)
