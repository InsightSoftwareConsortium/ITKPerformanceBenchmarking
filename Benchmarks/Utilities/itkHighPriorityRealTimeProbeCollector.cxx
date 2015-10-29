/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkHighPriorityRealTimeProbeCollector.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include "itkMultiThreader.h"


namespace itk
{
/** Constructor */
HighPriorityRealTimeProbeCollector
::HighPriorityRealTimeProbeCollector()
{
  this->GetSystemInformation();
//  this->Reset();
  this->m_TypeString                  = "Time";
  this->m_UnitString                  = "sec";
  this->m_HighPriorityRealTimeClock   = HighPriorityRealTimeClock::New();
  m_ProbeList.clear();
}

/** Destructor */
HighPriorityRealTimeProbeCollector
::~HighPriorityRealTimeProbeCollector()
{}

/** Set name of target */
void
HighPriorityRealTimeProbeCollector
::SetNameOfOverallProbe(std::string nameOfProbe)
{
  this->m_NameOfProbe = nameOfProbe;
}

/** Set number of threads */
void
HighPriorityRealTimeProbeCollector
::SetMumberOfThreads(const unsigned int numthreads)
{
  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(numthreads);
}

/** Reset */
void
HighPriorityRealTimeProbeCollector
::Reset(std::string probeName)
{
  this->FindHighPriorityRealTimeProbeWithName(probeName).Reset();
}

/** Get Name of this class */
const char *
HighPriorityRealTimeProbeCollector
::GetNameOfClass()
{
  return "HighPriorityRealTimeProbeCollector";
}

/** Returns the type probed value */
std::string
HighPriorityRealTimeProbeCollector
::GetType() const
{
  return this->m_TypeString;
}

/** Returns the unit probed value */
std::string
HighPriorityRealTimeProbeCollector
::GetUnit() const
{
  return this->m_UnitString;
}

/** Get the current time. */
HighPriorityRealTimeProbeCollector::TimeStampType
HighPriorityRealTimeProbeCollector
::GetInstantValue() const
{
  return m_HighPriorityRealTimeClock->GetTimeInSeconds();
}

/** Start counting */
void
HighPriorityRealTimeProbeCollector
::Start(std::string probeName)
{
  ProbeType tempprobe;
  if(this->CheckHighPriorityRealTimeProbeWithName(probeName,tempprobe))
    {
    tempprobe.Start();
    }
  else
    {
    tempprobe.Start();
    tempprobe.SetNameOfProbe(probeName);
    this->m_ProbeList.insert(std::make_pair(probeName,tempprobe));
    }
}

/** Stop the probe */
void
HighPriorityRealTimeProbeCollector
::Stop(std::string probeName)
{
  this->FindHighPriorityRealTimeProbeWithName(probeName).Stop();
}

/** Get Number of Starts */
HighPriorityRealTimeProbeCollector::CountType
HighPriorityRealTimeProbeCollector
::GetNumberOfStarts(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).GetNumberOfStarts();
}

/** Get Number of Stops */
HighPriorityRealTimeProbeCollector::CountType
HighPriorityRealTimeProbeCollector
::GetNumberOfStops(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).GetNumberOfStops();
}

/** Get Total */
HighPriorityRealTimeProbeCollector::TimeStampType
HighPriorityRealTimeProbeCollector
::GetTotal(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).GetTotal();
}

/** Get Min */
HighPriorityRealTimeProbeCollector::TimeStampType
HighPriorityRealTimeProbeCollector
::GetMin(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).GetMin();
}

/** Get Total */
HighPriorityRealTimeProbeCollector::TimeStampType
HighPriorityRealTimeProbeCollector
::GetMax(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).GetMax();
}

/** Get Mean */
HighPriorityRealTimeProbeCollector::TimeStampType
HighPriorityRealTimeProbeCollector
::GetMean(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).GetMean();
}

/** Get Standard deviation */
HighPriorityRealTimeProbeCollector::TimeStampType
HighPriorityRealTimeProbeCollector
::GetStandardDeviation(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).GetStandardDeviation();
}

/** Check validation of measurements*/
bool
HighPriorityRealTimeProbeCollector
::CheckValidation(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(probeName).CheckValidation();
}

/** Print System information */
void HighPriorityRealTimeProbeCollector
::PrintSystemInformation(std::ostream & os)
{
  os << "System:              " << m_SystemName << std::endl;
  os << "Processor:           " << m_ProcessorName << std::endl;
  os << "    Cache:           " << m_ProcessorCacheSize << std::endl;
  os << "    Clock:           " << m_ProcessorClockFrequency << std::endl;
  os << "    Cores:           " << m_NumberOfPhysicalCPU
     << " cpus x " << m_NumberOfLogicalCPU
     << " Cores = "<< m_NumberOfAvailableCore << std::endl;
  // Retrieve memory information in megabyte.
  os << "    Virtual Memory:  Total: "
     << m_TotalVirtualMemory
     <<" Available: "<< m_AvailableVirtualMemory << std::endl;
  os << "    Physical Memory: Total:"
     << m_TotalPhysicalMemory
     <<" Available: "<< m_AvailablePhysicalMemory << std::endl;

  os << "OSName:              "<< m_OSName << std::endl;
  os << "    Release:         "<< m_OSRelease << std::endl;
  os << "    Version:         "<< m_OSVersion << std::endl;
  os << "    Platform:        "<< m_OSPlatform << std::endl;

  os << "    Operating System is "
     << (m_Is64Bits?"64 bit":"32 bit") << std::endl;

  os << "ITK Version: "
     << m_ITKVersion << std::endl;
}

/** Print Probe Results. */
void
HighPriorityRealTimeProbeCollector
::PrintReportHead(std::ostream & os)
{
  std::stringstream ss;
  ss << std::setw( namewide ) << "Name Of Probe"
     << std::setw( tabwide  ) << "Iteration"
     << std::setw( tabwide  ) << "# Threads"
     << std::setw( tabwide  ) << "Total(sec)"
     << std::setw( tabwide  ) << "Min(sec)"
     << std::setw( tabwide  ) << "Mean(sec)"
     << std::setw( tabwide  ) << "Max(sec)"
     << std::setw( tabwide  ) << "Std(sec)";

  os << ss.str() << std::endl;
}

/** Print Probe Results. */
void
HighPriorityRealTimeProbeCollector
::PrintReport(std::string probeName, std::ostream & os,
              bool printSystemInfo, bool printReportHead )
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintReportHead();
    }

  this->FindHighPriorityRealTimeProbeWithName(probeName).PrintReport(os, false, false);
}

/** Print Probe Results. */
void
HighPriorityRealTimeProbeCollector
::PrintExpandedReportHead(std::ostream & os)
{
  std::stringstream ss;
  ss << std::setw( namewide ) << "Name Of Probe"
     << std::setw( tabwide  ) << "Iteration"
     << std::setw( tabwide  ) << "# Threads"
     << std::setw( tabwide  ) << "Total(sec)"
     << std::setw( tabwide  ) << "Best(sec)"
     << std::setw( tabwide  ) << "Best(diff)"
     << std::setw( tabwide  ) << "Best(%)"
     << std::setw( tabwide  ) << "Mean(sec)"
     << std::setw( tabwide  ) << "Worst(diff)"
     << std::setw( tabwide  ) << "Worst(%)"
     << std::setw( tabwide  ) << "Worst(sec)"
     << std::setw( tabwide +5 ) << "Total Diff(sec)"
     << std::setw( tabwide  ) << "Std(sec)";

  os << ss.str() << std::endl;
}

/** Print Probe Results. */
void
HighPriorityRealTimeProbeCollector
::PrintExpandedReport(std::string probeName,std::ostream & os,
                      bool printSystemInfo, bool printReportHead )
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintExpandedReportHead();
    }

  this->FindHighPriorityRealTimeProbeWithName(probeName).PrintExpandedReport(os, false, false);

}

/** Update the Min and Max values with an input value */
HighPriorityRealTimeProbeCollector::ProbeType&
HighPriorityRealTimeProbeCollector
::FindHighPriorityRealTimeProbeWithName(std::string & probeName)
{
  ProbeListType::iterator it = m_ProbeList.find(probeName);
  if(it != m_ProbeList.end())
    {
      return it->second;
    }
  else
    {
      itkExceptionMacro("No instance for Probe with the given name(" << probeName << "0");
    }
}

bool
HighPriorityRealTimeProbeCollector
::CheckHighPriorityRealTimeProbeWithName(std::string & probeName,ProbeType& probe)
{
  ProbeListType::iterator it = m_ProbeList.find(probeName);
  if(it != m_ProbeList.end())
    {
      probe = it->second;
      return true;
    }
  else
    {
      return false;
    }
}

/** Get System information */
void
HighPriorityRealTimeProbe
::GetSystemInformation()
{
  m_SystemInformation.RunCPUCheck();
  m_SystemInformation.RunMemoryCheck();
  m_SystemInformation.RunOSCheck();

  m_SystemName              = m_SystemInformation.GetHostname();
  m_ProcessorName           = m_SystemInformation.GetExtendedProcessorName();
  m_ProcessorCacheSize      = m_SystemInformation.GetProcessorCacheSize();
  m_ProcessorClockFrequency = m_SystemInformation.GetProcessorClockFrequency();
  m_NumberOfPhysicalCPU     = m_SystemInformation.GetNumberOfPhysicalCPU();
  m_NumberOfLogicalCPU      = m_SystemInformation.GetNumberOfLogicalCPU();
  m_NumberOfAvailableCore   = m_NumberOfPhysicalCPU*m_NumberOfLogicalCPU;

  m_OSName                  = m_SystemInformation.GetOSName();
  m_OSRelease               = m_SystemInformation.GetOSRelease();
  m_OSVersion               = m_SystemInformation.GetOSVersion();
  m_OSPlatform              = m_SystemInformation.GetOSPlatform();

  m_Is64Bits                = m_SystemInformation.Is64Bits();
  m_ITKVersion              = std::to_string(ITK_VERSION_MAJOR) +
                              std::string(".") +
                              std::to_string(ITK_VERSION_MINOR) +
                              std::string(".") +
                              std::to_string(ITK_VERSION_PATCH) +
                              std::string(".");

 // Retrieve memory information in megabyte.
  m_TotalVirtualMemory      = m_SystemInformation.GetTotalVirtualMemory();
  m_AvailableVirtualMemory  = m_SystemInformation.GetAvailableVirtualMemory();
  m_TotalPhysicalMemory     = m_SystemInformation.GetTotalPhysicalMemory();
  m_AvailablePhysicalMemory = m_SystemInformation.GetAvailablePhysicalMemory();

}

} // end namespace itk
