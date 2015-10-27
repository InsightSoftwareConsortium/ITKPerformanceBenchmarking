message("ClockTest")

add_executable(TimeTest TimeTest.cxx )
target_link_libraries(TimeTest ${ITK_LIBRARIES} )
add_test(NAME ClockTest COMMAND TimeTest )

add_executable(itkTimeProbeTest itkTimeProbeTest.cxx )
target_link_libraries(itkTimeProbeTest ${ITK_LIBRARIES} )
add_test(NAME itkTimeProbeTest COMMAND itkTimeProbeTest )

add_executable(itkTimeProbesTest itkTimeProbesTest.cxx )
target_link_libraries(itkTimeProbesTest ${ITK_LIBRARIES} )
add_test(NAME itkTimeProbesTest COMMAND itkTimeProbesTest )
