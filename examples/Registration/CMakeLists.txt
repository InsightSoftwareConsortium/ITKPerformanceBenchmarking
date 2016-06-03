project(ITKBenchmarkRegistration)

find_package(ITK REQUIRED
  COMPONENTS
    PerformanceBenchmarking
    ITKIOImageBase
    ITKIOMeta
    ITKIONRRD
    ITKIOTransformBase
    ITKIOTransformHDF5
    ITKRegistrationMethodsv4
    ITKMetricsv4
    ITKPDEDeformableRegistration
  )
include(${ITK_USE_FILE})

add_executable(RegistrationFrameworkBenchmark RegistrationFrameworkBenchmark.cxx)
target_link_libraries(RegistrationFrameworkBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME RegistrationFrameworkBenchmark
  COMMAND RegistrationFrameworkBenchmark
    ${TEST_OUTPUT_DIR}/RegistrationFrameworkBenchmark.tsv
    -1
    ${BRAIN_IMAGE}
    DATA{../Data/Input/brainweb165a10f17translated-7x-8y9z.nrrd}
    ${TEST_OUTPUT_DIR}/RegistrationFrameworkBenchmark.hdf5
  )
set_property(TEST RegistrationFrameworkBenchmark APPEND PROPERTY LABELS Registration)

add_executable(DemonsRegistrationBenchmark DemonsRegistrationBenchmark.cxx)
target_link_libraries(DemonsRegistrationBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME DemonsRegistrationBenchmark
  COMMAND DemonsRegistrationBenchmark
    ${TEST_OUTPUT_DIR}/DemonsRegistrationBenchmark.tsv
    -1
    ${BRAIN_IMAGE}
    DATA{../Data/Input/brainweb165a10f17translated-1x-1y1z.nrrd}
    ${TEST_OUTPUT_DIR}/DemonsRegistrationBenchmark.mha
  )
set_property(TEST DemonsRegistrationBenchmark APPEND PROPERTY LABELS Registration)
