//
// Created by Hans J. Johnson on 2/19/18.
//

#ifndef PERFORMANCEBENCHMARKING_PERFORMANCEBENCHMARKINGUTILITIES_H
#define PERFORMANCEBENCHMARKING_PERFORMANCEBENCHMARKINGUTILITIES_H

#include "jsonxx.h"
#include <ctime> //TODO:  Move to utiliites
#include "itkHighPriorityRealTimeProbesCollector.h"

extern std::string PerfDateStamp();

extern std::string ReplaceOccurrence( std::string str,
                                     const  std::string && findvalue,
                                     const  std::string && replacevalue);

extern std::string DecorateWithBuildInformation( std::string  inputJson);

extern void WriteExpandedReport(const std::string &timingsFileName, itk::HighPriorityRealTimeProbesCollector &collector,
                                bool printSystemInfo, bool printReportHead, bool useTabs);


#endif //PERFORMANCEBENCHMARKING_PERFORMANCEBENCHMARKINGUTILITIES_H
