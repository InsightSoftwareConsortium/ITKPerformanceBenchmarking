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
#ifndef itkResourceExpandedProbesCollector_hxx
#define itkResourceExpandedProbesCollector_hxx

#include "itkResourceExpandedProbesCollector.h"
#include <iostream>

namespace itk
{

template< typename TProbe >
ResourceExpandedProbesCollector< TProbe >
::ResourceExpandedProbesCollector()
{
  SuperClass::Clear();
  this->GetSystemInformation();
  this->EnableUpdatingProbeNameWithNumOfThreads(false);
}

template< typename TProbe >
ResourceExpandedProbesCollector< TProbe >
::~ResourceExpandedProbesCollector()
{}


/** Start a probe with a particular name. If the time probe does not
 * exist, it will be created */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::Start(const char *name)
{
  std::string updatedname = this->UpdateProbeNameWithNumOfThreads(name).c_str();
  SuperClass::Start(updatedname.c_str());
  this->m_Probes[updatedname].SetNameOfProbe(updatedname.c_str());
}

/** Stop a time probe identified with a name */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::Stop(const char *name)
{
  SuperClass::Stop(
              this->UpdateProbeNameWithNumOfThreads(name).c_str());
}

/** Set name of target */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::SetNameOfOverallProbe(const char* nameOfProbe)
{
  this->m_NameOfProbe = nameOfProbe;
}

/** Set number of threads */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::SetNumberOfThreads(const unsigned int numthreads)
{
  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(numthreads);
}

/** Print System information */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
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


/** Report the summary of results from the probes */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::Report(std::ostream & os)
{
  // If the TProbe is child class of itkResourceProbe,
  // There is no report function
  //ResourceProbesCollectorBase<TProbe>::Report(os);

  // If the TProbe is inherited from itkExpandedResourceProbe
  // There is a report function.
  //this->ExpandedReport(os);
  this->ExpandedReportAll(os, true, true);
}

/** Report the summary of results from the probes */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::ExpandedReportAll(std::ostream & os,
         bool printSystemInfo ,bool printReportHead )
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintExpandedReportHead();
    }

  typename MapType::iterator it        = this->m_Probes.begin();
  typename MapType::const_iterator end = this->m_Probes.end();

  while(it != end)
    {
    it->second.ExpandedReport(os, false, false);
    ++it;
    }
}

/** Print a specific time probe's results. */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::ExpandedReport(const char* probeName,std::ostream & os,
         bool printSystemInfo ,bool printReportHead)
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintExpandedReportHead();
    }

  FindProbeWithName(probeName).ExpandedReport(os, false, false);
}

/** Print all time probes' expanded results. */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::AnalysisReportAll(std::ostream & os,
                 bool printSystemInfo,bool printReportHead)
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintAnalysisReportHead();
    }

  typename MapType::iterator it        = this->m_Probes.begin();
  typename MapType::const_iterator end = this->m_Probes.end();

  while(it != end)
    {
    it->second.AnalysisReport(os, false, false);
    ++it;
    }
}

/** Print a specific time probe's expanded results. */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::AnalysisReport(const char* probeName,std::ostream & os,
                 bool printSystemInfo,bool printReportHead)
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation();
    }

  if(printReportHead)
    {
    this->PrintAnalysisReportHead();
    }

  FindProbeWithName(probeName).AnalysisReport(os, false, false);
}

/** Set enabling the funtion of UpdateProbeNameWithNumOfThreads */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::EnableUpdatingProbeNameWithNumOfThreads(bool updateProbeName)
{
  this->m_UpdateProbeNameWithNumOfThreads = updateProbeName;
}

/** Update a probe name with the number of threads. */
template< typename TProbe >
std::string
ResourceExpandedProbesCollector< TProbe >
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

/** find a probe with a specific name. */
template< typename TProbe >
TProbe&
ResourceExpandedProbesCollector< TProbe >
::FindProbeWithName(const char* probeName)
{
  IdType tid = this->UpdateProbeNameWithNumOfThreads(probeName);
  typename MapType::const_iterator end = this->m_Probes.end();
  typename MapType::iterator pos = this->m_Probes.find(tid);

  if ( pos == end )
    {
    itkGenericExceptionMacro(<< "The probe \"" << probeName << "\" does not exist. It can not be stopped.");
    return;
    }

  return pos;
}

/** Print Probe Results. */
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::PrintExpandedReportHead(std::ostream & os)
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
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::PrintAnalysisReportHead(std::ostream & os)
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
template< typename TProbe >
void
ResourceExpandedProbesCollector< TProbe >
::GetSystemInformation()
{
  itksys::SystemInformation systeminfo;
  systeminfo.RunCPUCheck();
  systeminfo.RunMemoryCheck();
  systeminfo.RunOSCheck();

  m_SystemName              = systeminfo.GetHostname();
  m_ProcessorName           = systeminfo.GetExtendedProcessorName();
  m_ProcessorCacheSize      = systeminfo.GetProcessorCacheSize();
  m_ProcessorClockFrequency = systeminfo.GetProcessorClockFrequency();
  m_NumberOfPhysicalCPU     = systeminfo.GetNumberOfPhysicalCPU();
  m_NumberOfLogicalCPU      = systeminfo.GetNumberOfLogicalCPU();
  m_NumberOfAvailableCore   = m_NumberOfPhysicalCPU*m_NumberOfLogicalCPU;

  m_OSName                  = systeminfo.GetOSName();
  m_OSRelease               = systeminfo.GetOSRelease();
  m_OSVersion               = systeminfo.GetOSVersion();
  m_OSPlatform              = systeminfo.GetOSPlatform();

  m_Is64Bits                = systeminfo.Is64Bits();
  m_ITKVersion              = std::to_string(ITK_VERSION_MAJOR) +
                                std::string(".") +
                                std::to_string(ITK_VERSION_MINOR) +
                                std::string(".") +
                                std::to_string(ITK_VERSION_PATCH) +
                                std::string(".");

  // Retrieve memory information in megabyte.
  m_TotalVirtualMemory      = systeminfo.GetTotalVirtualMemory();
  m_AvailableVirtualMemory  = systeminfo.GetAvailableVirtualMemory();
  m_TotalPhysicalMemory     = systeminfo.GetTotalPhysicalMemory();
  m_AvailablePhysicalMemory = systeminfo.GetAvailablePhysicalMemory();

}

} // end namespace itk

#endif //itkResourceProbesCollectorBase_hxx
