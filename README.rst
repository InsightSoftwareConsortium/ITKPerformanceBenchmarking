ITKPerformanceBenchmarking
==========================

.. image:: https://github.com/InsightSoftwareConsortium/ITKPerformanceBenchmarking/workflows/Build,%20test,%20package/badge.svg

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

Requirements
------------
- `CMake <https://cmake.org/>`_
- `Ninja <https://ninja-build.org/>`_
- `Python <https://www.python.org/>`_

Notes for running the benchmarks
--------------------------------

An example to call the benchmarking script is::

  $ python ./evaluate-itk-performance.py run -g {ITK-version} {ITK-source} {ITK-build} {ITKPerformanceBenchmarking-build}

where ``{ITK-version}`` is the ITK version that the user wishes to evaluate.

Also, note that the ITK source folder ``(ITK-source}``, where the specific
version is fetched, needs to exist.

The generated/result ``JSON`` files are placed in::

  ./{ITKPerformanceBenchmarking-build}/BenchmarkResults/{machine-name}


Notes for benchmarking in Windows
---------------------------------

**CMake** and **Ninja** need to be in the **PATH**. Also, the **C++ compiler**
has to be in the *PATH* so that Ninja can find it.

For the Microsoft Visual Studio compiler, the C++ compiler is a file that is
usually under::

  C:/Program Files/Microsoft Visual Studio {version}/VC/bin/cl.exe

The user will need to change the path to the *vcvars\*.bat* command file in the
`RunWithVisualStudio.cmd <https://github.com/InsightSoftwareConsortium/ITKPerformanceBenchmarking/blob/3a880e47e99cd0f429771960a4cee4fd70873ec6/RunWithVisualStudio.cmd#L1>`_ command line script to the specific location of their
*vcvars* file, e.g.::

  C:/Program Files/Microsoft Visual Studio {version}/VC/vcvarsall.bat

Finally, the user will need to start the Git bash by double-clicking on the
``RunWithVisualStudio.cmd`` script.

Note that the module is built with static libraries to allow for ITK
benchmarking in Windows: the ``BUILD_SHARED_LIBS`` flag in the
`evaluate-itk-performance.py <https://github.com/InsightSoftwareConsortium/ITKPerformanceBenchmarking/blob/3a880e47e99cd0f429771960a4cee4fd70873ec6/evaluate-itk-performance.py#L129>`_ script is set to `OFF`.


License
-------

This software is distributed under the Apache 2.0 license. Please see
the *LICENSE* file for details.
