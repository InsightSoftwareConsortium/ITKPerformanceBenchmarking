project(ITKBenchmarkRegistration)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarking
    ITKIOImageBase
    ITKIOMeta
    ITKIONRRD
    ITKRegistrationMethodsv4
    ITKMetricsv4
  )
include(${ITK_USE_FILE})

add_executable(RegistrationFrameworkBenchmark RegistrationFrameworkBenchmark.cxx)
target_link_libraries(RegistrationFrameworkBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME RegistrationFrameworkBenchmark
  COMMAND RegistrationFrameworkBenchmark
    ${BRAIN_IMAGE}
    DATA{../Data/Input/brainweb165a10f17translated-7x-8y9z.nrrd}
    ${TEST_OUTPUT_DIR}/RegistrationFrameworkBenchmark.tsv
    ${TEST_OUTPUT_DIR}/RegistrationFrameworkBenchmark.mha
  )
set_property(TEST RegistrationFrameworkBenchmark APPEND PROPERTY LABELS Registration)
