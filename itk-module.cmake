# the top-level README is used for describing this module, just
# re-used it for documentation here
get_filename_component(MY_CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(READ "${MY_CURRENT_DIR}/README.md" DOCUMENTATION)

set(DOCUMENTATION
       	"New classes increase operating system process priority to
minimize the impact of other processes running on the system.

These classes are used by a used by a suite of example ITK benchmarks to
quantify toolkit performance.

For more information, see::

  McCormick M., Kang H.J., Barre S.
  Performance Benchmarking the Insight Toolkit
  The Insight Journal. January-December. 2016.
  https://hdl.handle.net/10380/3557
  https://insight-journal.org/browse/publication/972
")

itk_module(PerformanceBenchmarking
  ENABLE_SHARED
  PRIVATE_DEPENDS
    ITKCommon
  TEST_DEPENDS
    ITKTestKernel
  EXCLUDE_FROM_DEFAULT
  DESCRIPTION
  "${DOCUMENTATION}"
)
