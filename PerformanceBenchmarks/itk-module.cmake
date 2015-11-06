set(DOCUMENTATION "ITK Performance Benchmarks")

itk_module(PerformanceBenchmarks
  ENABLE_SHARED
  PRIVATE_DEPENDS
    ITKCommon
  TEST_DEPENDS
    ITKTestKernel
    ITKCommon
  DESCRIPTION
  "${DOCUMENTATION}"
)
