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
#include "itkHighPriorityRealTimeProbe.h"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "itkMultiThreader.h"

namespace itk
{
/** Constructor */
HighPriorityRealTimeProbe
::HighPriorityRealTimeProbe()
{
  this->GetSystemInformation();
  this->Reset();
  this->m_TypeString                  = "Time";
  this->m_UnitString                  = "sec";
  this->m_HighPriorityRealTimeClock   = HighPriorityRealTimeClock::New();
}

/** Destructor */
HighPriorityRealTimeProbe
::~HighPriorityRealTimeProbe()
{}

/** Set name of target */
void
HighPriorityRealTimeProbe
::SetNameOfProbe(std::string nameOfProbe)
{
  this->m_NameOfProbe = nameOfProbe;
}

/** Set number of threads */
void
HighPriorityRealTimeProbe
::SetMumberOfThreads(const unsigned int numthreads)
{
  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(numthreads);
}

/** Reset */
void
HighPriorityRealTimeProbe
::Reset()
{
  this->m_TotalValue        = NumericTraits< TimeStampType >::ZeroValue();
  this->m_StartValue        = NumericTraits< TimeStampType >::ZeroValue();
  this->m_MinValue          = NumericTraits< TimeStampType >::ZeroValue();
  this->m_MaxValue          = NumericTraits< TimeStampType >::ZeroValue();
  this->m_MeanValue         = NumericTraits< TimeStampType >::ZeroValue();
  this->m_StandardDeviation = NumericTraits< TimeStampType >::ZeroValue();

  this->m_NumberOfStarts    = NumericTraits< CountType >::ZeroValue();
  this->m_NumberOfStops     = NumericTraits< CountType >::ZeroValue();
  this->m_NumberOfIteration = NumericTraits< CountType >::ZeroValue();

  this->m_ElapsedTimeList.clear();
}

/** Returns the type probed value */
std::string
HighPriorityRealTimeProbe
::GetType(void) const
{
  return this->m_TypeString;
}

/** Returns the unit probed value */
std::string
HighPriorityRealTimeProbe
::GetUnit(void) const
{
  return this->m_UnitString;
}

/** Get the current time. */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetInstantValue(void) const
{
  return m_HighPriorityRealTimeClock->GetTimeInSeconds();
}

/** Start counting */
void
HighPriorityRealTimeProbe
::Start()
{
  this->m_NumberOfStarts++;
  this->m_StartValue = this->GetInstantValue();
}

/** Stop the probe */
void
HighPriorityRealTimeProbe
::Stop()
{
  if ( this->m_NumberOfStops == this->m_NumberOfStarts )
    {
    return;
    }

  TimeStampType elapsedtime = this->GetInstantValue() - this->m_StartValue;
  this->m_ElapsedTimeList.push_back(elapsedtime);
  // Update Measurement
  this->UpdatekMinMaxValue(elapsedtime);
  this->m_TotalValue += elapsedtime;
  this->m_NumberOfStops++;
  this->m_NumberOfIteration =
  static_cast<CountType>(this->m_ElapsedTimeList.size());
}

/** Get Number of Starts */
HighPriorityRealTimeProbe::CountType
HighPriorityRealTimeProbe
::GetNumberOfStarts() const
{
  return this->m_NumberOfStarts;
}

/** Get Number of Stops */
HighPriorityRealTimeProbe::CountType
HighPriorityRealTimeProbe
::GetNumberOfStops() const
{
  return this->m_NumberOfStops;
}

/** Get Total */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetTotal() const
{
  return this->m_TotalValue;
}

/** Get Min */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetMin() const
{
  return this->m_MinValue;
}

/** Get Total */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetMax() const
{
  return this->m_MaxValue;
}

/** Get Mean */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetMean()
{
  this->m_MeanValue =
    this->m_NumberOfIteration > 0 ?
    m_TotalValue/static_cast<TimeStampType>(this->m_ElapsedTimeList.size()) :
    NumericTraits< TimeStampType >::ZeroValue();
  return this->m_MeanValue;
}

/** Get Standard deviation */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetStandardDeviation()
{
  std::vector<TimeStampType> diff(this->m_ElapsedTimeList.size());
  std::transform(this->m_ElapsedTimeList.begin(),
                 this->m_ElapsedTimeList.end(),
                 diff.begin(),
                 std::bind2nd(std::minus<TimeStampType>(),
                              this->m_MeanValue));
  TimeStampType sq_sum =
    std::inner_product(diff.begin(),diff.end(),
                       diff.begin(),
                       0.0);

  int sz = this->m_ElapsedTimeList.size()-1;
  if (sz <=0)
    {
    this->m_StandardDeviation = NumericTraits< TimeStampType >::ZeroValue();
    }
  else
    {
    this->m_StandardDeviation =
      std::sqrt(sq_sum /
               (static_cast<TimeStampType>(sz)));
    }
  return this->m_StandardDeviation;
}

/** Check validation of measurements*/
bool
HighPriorityRealTimeProbe
::CheckValidation()
{
  if((this->m_NumberOfIteration == this->m_NumberOfStarts )
     && (this->m_NumberOfIteration == this->m_NumberOfStops))
    {
    return true;
    }
  return false;
}

/** Print System information */
void HighPriorityRealTimeProbe
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
HighPriorityRealTimeProbe
::Report(std::ostream & os, bool printSystemInfo, bool printReportHead )
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation(os);
    }

  if(printReportHead)
    {
    this->PrintReportHead(os);
    }

  std::stringstream ss;
  ss << std::setw( namewide ) << this->m_NameOfProbe
     << std::setw( tabwide  ) << this->m_NumberOfIteration
     << std::setw( tabwide  ) << itk::MultiThreader::GetGlobalDefaultNumberOfThreads()
     << std::setw( tabwide  ) << this->GetTotal()
     << std::setw( tabwide  ) << this->GetMin()
     << std::setw( tabwide  ) << this->GetMean()
     << std::setw( tabwide  ) << this->GetMax()
     << std::setw( tabwide  ) << this->GetStandardDeviation();
  os << ss.str() << std::endl;
}

/** Print Probe Results. */
void
HighPriorityRealTimeProbe
::ExpandedReport(std::ostream & os, bool printSystemInfo, bool printReportHead )
{
  if(printSystemInfo)
    {
    this->PrintSystemInformation(os);
    }

  if(printReportHead)
    {
    this->PrintExpandedReportHead(os);
    }

  std::stringstream ss;
  ss << std::setw( namewide ) << this->m_NameOfProbe
     << std::setw( tabwide  ) << this->m_NumberOfIteration
     << std::setw( tabwide  ) << itk::MultiThreader::GetGlobalDefaultNumberOfThreads()
     << std::setw( tabwide  ) << this->GetTotal()
     << std::setw( tabwide  ) << this->GetMin()
     << std::setw( tabwide  ) << this->GetMean() - this->GetMin()
     << std::setw( tabwide  ) << (this->GetMean()/this->GetMin())*100
     << std::setw( tabwide  ) << this->GetMean()
     << std::setw( tabwide  ) << this->GetMax() - this->GetMean()
     << std::setw( tabwide  ) << (this->GetMax()/this->GetMean())*-100
     << std::setw( tabwide  ) << this->GetMax()
     << std::setw( tabwide +5 ) << this->GetMax() - this->GetMin()
     << std::setw( tabwide  ) << this->GetStandardDeviation();
  os << ss.str() << std::endl;
}

/** Update the Min and Max values with an input value */
void
HighPriorityRealTimeProbe
::UpdatekMinMaxValue(TimeStampType& value)
{
  if(this->m_NumberOfStops == 0)
    {
    this->m_MinValue = value;
    this->m_MaxValue = value;
    }
  else
    {
    this->m_MinValue = this->m_MinValue > value ? value : this->m_MinValue;
    this->m_MaxValue = this->m_MaxValue < value ? value : this->m_MaxValue;
    }
}

/** Print Probe Results. */
void
HighPriorityRealTimeProbe
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
HighPriorityRealTimeProbe
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
HighPriorityRealTimeProbe
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
