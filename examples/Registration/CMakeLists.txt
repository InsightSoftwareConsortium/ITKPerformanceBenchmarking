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
    ITKConvolution
  )
include(${ITK_USE_FILE})

add_executable(RegistrationFrameworkBenchmark RegistrationFrameworkBenchmark.cxx)
target_link_libraries(RegistrationFrameworkBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME RegistrationFrameworkBenchmark
  COMMAND RegistrationFrameworkBenchmark
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__RegistrationFrameworkBenchmark.json
    3
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
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__DemonsRegistrationBenchmark.json
    3
    -1
    ${BRAIN_IMAGE}
    DATA{../Data/Input/brainweb165a10f17translated-1x-1y1z.nrrd}
    ${TEST_OUTPUT_DIR}/DemonsRegistrationBenchmark.mha
  )
set_property(TEST DemonsRegistrationBenchmark APPEND PROPERTY LABELS Registration)

add_executable(NormalizedCorrelationBenchmark NormalizedCorrelationBenchmark.cxx)
target_link_libraries(NormalizedCorrelationBenchmark ${ITK_LIBRARIES})
ExternalData_Add_Test(ITKBenchmarksData
  NAME NormalizedCorrelationBenchmark
  COMMAND NormalizedCorrelationBenchmark
    ${BENCHMARK_RESULTS_OUTPUT_DIR}/__DATESTAMP__NormalizedCorrelationBenchmark.json
    3
    -1
    DATA{../Data/Input/brainweb165a10f17extract88i5z.mha}
    DATA{../Data/Input/brainweb165a10f17translated-1x-1y1zextract88i5z.mha}
  )
set_property(TEST NormalizedCorrelationBenchmark APPEND PROPERTY LABELS Registration)
## performance tests should not be run in parallel
set_tests_properties(RegistrationFrameworkBenchmark DemonsRegistrationBenchmark NormalizedCorrelationBenchmark PROPERTIES RUN_SERIAL TRUE)
