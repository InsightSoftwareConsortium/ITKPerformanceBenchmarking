//
// Created by Hans J. Johnson on 2/19/18.
//

#ifndef PerformanceBenchmarkingUtilities_h
#define PerformanceBenchmarkingUtilities_h

#include "PerformanceBenchmarkingInformation.h"

#include "jsonxx.h"
#include <ctime> //TODO:  Move to utiliites
#include "itkHighPriorityRealTimeProbesCollector.h"

#if ITK_VERSION_MAJOR < 5 || defined(ITK_USES_NUMEROFTHREADS)
#include "itkMultiThreader.h"
using MultiThreaderName = itk::MultiThreader;
#define SET_PARALLEL_UNITS( x ) SetNumberOfThreads( x )
#else
#include "itkMultiThreaderBase.h"
using MultiThreaderName = itk::MultiThreaderBase;
#define SET_PARALLEL_UNITS( x ) SetNumberOfWorkUnits( x )
#endif

PerformanceBenchmarking_EXPORT
std::string
PerfDateStamp();

PerformanceBenchmarking_EXPORT
std::string
ReplaceOccurrence( std::string str,
                   const  std::string && findvalue,
                   const  std::string && replacevalue );

PerformanceBenchmarking_EXPORT
std::string
DecorateWithBuildInformation( std::string inputJson);

PerformanceBenchmarking_EXPORT
void
WriteExpandedReport( const std::string &timingsFileName,
                     itk::HighPriorityRealTimeProbesCollector &collector,
                     bool printSystemInfo,
                     bool printReportHead,
                     bool useTabs );
#endif
