ITKPerformanceBenchmarking
==========================

.. image:: https://circleci.com/gh/InsightSoftwareConsortium/ITKPerformanceBenchmarking/tree/master.svg?style=svg
    :target: https://circleci.com/gh/InsightSoftwareConsortium/ITKPerformanceBenchmarking/tree/master

Real-world tests to benchmark ITK performance.

New classes increase operating system process priority to
minimize the impact of other processes running on the system.

These classes are used by a suite of example ITK benchmarks to quantify toolkit
performance.

For more information, see the `Insight Journal article <http://hdl.handle.net/10380/3557>`_::

  McCormick M., Kang H.J., Barre S.
  Performance Benchmarking the Insight Toolkit
  The Insight Journal. January-December. 2016.
  http://hdl.handle.net/10380/3557
  http://insight-journal.org/browse/publication/972

Since ITK 4.11.0, this module is available in the ITK source tree as a Remote
module.  To enable it, set::

  Module_PerformanceBenchmarking:BOOL=ON

in ITK's CMake build configuration.


# Driving performance benchmarks for a given ITK source tree

  See ITK_PerformanceTestingDriver.sh in this directory
