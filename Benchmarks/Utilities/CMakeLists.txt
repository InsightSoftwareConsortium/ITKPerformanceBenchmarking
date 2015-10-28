PROJECT( ITKBenchmarkUtilities )

FIND_PACKAGE( ITK )
IF( ITK_FOUND )
  INCLUDE( ${ITK_USE_FILE} )

  SET( ITKBenchmarkUtilities_INCLUDE_FILES
    itkHighPriorityRealTimeClock.cxx
    itkHighPriorityRealTimeProbe.cxx
    )

  SET( ITKBenchmarkUtilities_SOURCE_FILES
    itkHighPriorityRealTimeClock.cxx
    itkHighPriorityRealTimeProbe.cxx
    )

  ADD_LIBRARY( ITKBenchmarkUtilities
    ${ITKBenchmarkUtilities_INCLUDE_FILES}
    ${ITKBenchmarkUtilities_SOURCE_FILES}
    )

  TARGET_LINK_LIBRARIES( ITKBenchmarkUtilities
    ITKCommon
    )

ENDIF( ITK_FOUND )
