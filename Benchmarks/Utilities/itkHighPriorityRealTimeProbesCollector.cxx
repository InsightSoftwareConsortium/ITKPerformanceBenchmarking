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
#include "itkHighPriorityRealTimeProbesCollector.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include "itkMultiThreader.h"


namespace itk
{
/** Constructor */
HighPriorityRealTimeProbesCollector
::HighPriorityRealTimeProbesCollector()
{
  this->GetSystemInformation();
//  this->Reset();
  this->m_TypeString                      = "Time";
  this->m_UnitString                      = "sec";
  this->m_HighPriorityRealTimeClock       = HighPriorityRealTimeClock::New();
  this->m_UpdateProbeNameWithNumOfThreads = true;
  this->m_ProbeList.clear();
}

/** Destructor */
HighPriorityRealTimeProbesCollector
::~HighPriorityRealTimeProbesCollector()
{}

/** Set name of target */
void
HighPriorityRealTimeProbesCollector
::SetNameOfOverallProbe(std::string nameOfProbe)
{
  this->m_NameOfProbe = nameOfProbe;
}

/** Set number of threads */
void
HighPriorityRealTimeProbesCollector
::SetMumberOfThreads(const unsigned int numthreads)
{
  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(numthreads);
}

/** Reset */
void
HighPriorityRealTimeProbesCollector
::Reset(std::string probeName)
{
  this->FindHighPriorityRealTimeProbeWithName(probeName).Reset();
}

/** Reset All probes*/
void
HighPriorityRealTimeProbesCollector
::Reset()
{
  ProbeListType::iterator       it  = this->m_ProbeList.begin();
  ProbeListType::const_iterator end = this->m_ProbeList.end();
  while(it != end)
    {
    it->second.Reset();
    ++it;
    }
}

/** Get Name of this class */
const char *
HighPriorityRealTimeProbesCollector
::GetNameOfClass()
{
  return "HighPriorityRealTimeProbeCollector";
}

/** Returns the type probed value */
std::string
HighPriorityRealTimeProbesCollector
::GetType() const
{
  return this->m_TypeString;
}

/** Returns the unit probed value */
std::string
HighPriorityRealTimeProbesCollector
::GetUnit() const
{
  return this->m_UnitString;
}

/** Get the current time. */
HighPriorityRealTimeProbesCollector::TimeStampType
HighPriorityRealTimeProbesCollector
::GetInstantValue() const
{
  return m_HighPriorityRealTimeClock->GetTimeInSeconds();
}

/** Start counting */
void
HighPriorityRealTimeProbesCollector
::Start(std::string probeName)
{
  ProbeType tempprobe;
  std::string tempname = UpdateProbeNameWithNumOfThreads(probeName);
  if(this->CheckHighPriorityRealTimeProbeWithName(tempname,tempprobe))
    {
    tempprobe.Start();
    }
  else
    {
    tempprobe.SetNameOfProbe(tempname);
    this->m_ProbeList.insert(std::make_pair(tempname,tempprobe));
    this->m_ProbeList[tempname].Start();
    }
}

/** Stop the probe */
void
HighPriorityRealTimeProbesCollector
::Stop(std::string probeName)
{
  this->FindHighPriorityRealTimeProbeWithName(UpdateProbeNameWithNumOfThreads(probeName)).Stop();
}

/** Get Number of Starts */
HighPriorityRealTimeProbesCollector::CountType
HighPriorityRealTimeProbesCollector
::GetNumberOfStarts(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).GetNumberOfStarts();
}

/** Get Number of Stops */
HighPriorityRealTimeProbesCollector::CountType
HighPriorityRealTimeProbesCollector
::GetNumberOfStops(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).GetNumberOfStops();
}

/** Get Total */
HighPriorityRealTimeProbesCollector::TimeStampType
HighPriorityRealTimeProbesCollector
::GetTotal(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).GetTotal();
}

/** Get Min */
HighPriorityRealTimeProbesCollector::TimeStampType
HighPriorityRealTimeProbesCollector
::GetMin(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).GetMin();
}

/** Get Total */
HighPriorityRealTimeProbesCollector::TimeStampType
HighPriorityRealTimeProbesCollector
::GetMax(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).GetMax();
}

/** Get Mean */
HighPriorityRealTimeProbesCollector::TimeStampType
HighPriorityRealTimeProbesCollector
::GetMean(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).GetMean();
}

/** Get Standard deviation */
HighPriorityRealTimeProbesCollector::TimeStampType
HighPriorityRealTimeProbesCollector
::GetStandardDeviation(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).GetStandardDeviation();
}

/** Check the validation of a specific time probe*/
bool
HighPriorityRealTimeProbesCollector
::CheckValidation(std::string probeName)
{
  return this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).CheckValidation();
}

/** Check the validation of all time probes*/
bool
HighPriorityRealTimeProbesCollector
::CheckValidation()
{
  bool temp(true);
  ProbeListType::iterator       it  = this->m_ProbeList.begin();
  ProbeListType::const_iterator end = this->m_ProbeList.end();
  while(it != end)
    {
    temp &= it->second.CheckValidation();
    ++it;
    }

  return temp;
}

/** Print System information */
void HighPriorityRealTimeProbesCollector
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

/** Print all time probes' results. */
void
HighPriorityRealTimeProbesCollector
::Report(std::ostream & os,bool printSystemInfo,bool printReportHead)
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintReportHead();
    }

  ProbeListType::iterator       it  = this->m_ProbeList.begin();
  ProbeListType::const_iterator end = this->m_ProbeList.end();
  while(it != end)
    {
    it->second.Report(os, false, false);
    ++it;
    }
}

/** Print a specific time probe's results. */
void
HighPriorityRealTimeProbesCollector
::Report(std::string probeName, std::ostream & os,
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

  this->FindHighPriorityRealTimeProbeWithName(
              UpdateProbeNameWithNumOfThreads(probeName)).Report(os, false, false);
}

/** Print all time probes' expanded results. */
void
HighPriorityRealTimeProbesCollector
::ExpandedReport(std::ostream & os,bool printSystemInfo,bool printReportHead)
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintReportHead();
    }

  ProbeListType::iterator       it  = this->m_ProbeList.begin();
  ProbeListType::const_iterator end = this->m_ProbeList.end();
  while(it != end)
    {
    it->second.ExpandedReport(os, false, false);
    ++it;
    }
}

/** Print a specific time probe's expanded results. */
void
HighPriorityRealTimeProbesCollector
::ExpandedReport(std::string probeName,std::ostream & os,
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

  this->FindHighPriorityRealTimeProbeWithName(
          UpdateProbeNameWithNumOfThreads(probeName)).ExpandedReport(os, false, false);
}

/** Set enabling the funtion of UpdateProbeNameWithNumOfThreads */
void
HighPriorityRealTimeProbesCollector
::EnableUpdatingProbeNameWithNumOfThreads(bool updateProbeName)
{
  this->m_UpdateProbeNameWithNumOfThreads = updateProbeName;
}

/** Update a probe name with the number of threads. */
std::string
HighPriorityRealTimeProbesCollector
::UpdateProbeNameWithNumOfThreads(std::string probeName)
{
   std::string updatedProbeName;
   if(this->m_UpdateProbeNameWithNumOfThreads)
   {
   updatedProbeName = probeName + std::string("_Th_") +
          std::to_string(itk::MultiThreader::GetGlobalDefaultNumberOfThreads());
   }
   else
   {
   updatedProbeName = probeName;
   }

   return updatedProbeName;
}

/** Update the Min and Max values with an input value */
HighPriorityRealTimeProbesCollector::ProbeType&
HighPriorityRealTimeProbesCollector
::FindHighPriorityRealTimeProbeWithName(std::string probeName)
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
HighPriorityRealTimeProbesCollector
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

/** Print Probe Results. */
void
HighPriorityRealTimeProbesCollector
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
HighPriorityRealTimeProbesCollector
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

/** Get System information */
void
HighPriorityRealTimeProbesCollector
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
