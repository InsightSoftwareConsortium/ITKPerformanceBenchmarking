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
#ifndef itkResourceExpandedProbe_hxx
#define itkResourceExpandedProbe_hxx

#include <numeric>
#include <iomanip>

#include "itkResourceExpandedProbe.h"
#include "itkMultiThreader.h"
#include "itksys/SystemInformation.hxx"

namespace itk
{
/** Constructor */
template< typename ValueType, typename MeanType >
ResourceExpandedProbe< ValueType, MeanType >
::ResourceExpandedProbe(const std::string & type, const std::string & unit):
  ResourceProbe2< ValueType, MeanType >(type,unit)
{
  this->GetSystemInformation();
  this->Reset();
}

/** Destructor */
template< typename ValueType, typename MeanType >
ResourceExpandedProbe< ValueType, MeanType >
::~ResourceExpandedProbe()
{}

/** Set name of target */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::SetNameOfProbe(const char* nameOfProbe)
{
  this->m_NameOfProbe = nameOfProbe;
}

/** Set number of threads */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::SetNumberOfThreads(const unsigned int numthreads)
{
  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(numthreads);
}

/** Reset */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::Reset()
{
  ResourceProbe2<ValueType,MeanType>::Reset();

  this->m_MinimumValue      = NumericTraits< ValueType >::ZeroValue();
  this->m_MaximumValue      = NumericTraits< ValueType >::ZeroValue();
  this->m_MeanValue         = NumericTraits< ValueType >::ZeroValue();
  this->m_StandardDeviation = NumericTraits< ValueType >::ZeroValue();
  this->m_NumberOfIteration = NumericTraits< CountType >::ZeroValue();
  this->m_NumberOfThread    = NumericTraits< CountType >::ZeroValue();
  this->m_ProbeValueList.clear();
}

/** Stop the probe */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::Start()
{
  this->IncreaseNumberOfStarts();
  this->m_NumberOfThread = itk::MultiThreader::GetGlobalDefaultNumberOfThreads();
  this->SetStartValue(this->GetInstantValue());
}


/** Stop the probe */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::Stop()
{
  ValueType probevalue = this->GetInstantValue() - this->GetStartValue();

  if ( this->GetNumberOfStops() == this->GetNumberOfStarts() )
    {
    return;
    }

  this->UpdatekMinimumMaximumMeasuredValue(probevalue);
  this->SetTotalValue(this->GetTotal() + probevalue);
  this->m_ProbeValueList.push_back(probevalue);
  this->IncreaseNumberOfStops();
  this->m_NumberOfIteration = static_cast<CountType>(this->m_ProbeValueList.size());
}

/** Get Min */
template< typename ValueType, typename MeanType >
ValueType
ResourceExpandedProbe< ValueType, MeanType >
::GetMinimum() const
{
  return this->m_MinimumValue;
}

/** Get Total */
template< typename ValueType, typename MeanType >
ValueType
ResourceExpandedProbe< ValueType, MeanType >
::GetMaximum() const
{
  return this->m_MaximumValue;
}

/** Get Mean */
template< typename ValueType, typename MeanType >
ValueType
ResourceExpandedProbe< ValueType, MeanType >
::GetMean() const
{
  return this->m_NumberOfIteration > 0 ?
         this->GetTotal()/static_cast<ValueType>(this->m_ProbeValueList.size()) :
         NumericTraits< ValueType >::ZeroValue();
}

/** Get Standard deviation */
template< typename ValueType, typename MeanType >
ValueType
ResourceExpandedProbe< ValueType, MeanType >
::GetStandardDeviation()
{
  this->m_MeanValue = this->GetMean();
  std::vector<ValueType> diff(this->m_ProbeValueList.size());
  std::transform(this->m_ProbeValueList.begin(),
                 this->m_ProbeValueList.end(),
                 diff.begin(),
                 std::bind2nd(std::minus<ValueType>(),
                              this->m_MeanValue ));
  ValueType sq_sum =
    std::inner_product(diff.begin(),diff.end(),
                       diff.begin(),
                       0.0);

  int sz = this->m_ProbeValueList.size()-1;
  if (sz <=0)
    {
    this->m_StandardDeviation = NumericTraits< ValueType >::ZeroValue();
    }
  else
    {
    this->m_StandardDeviation =
      std::sqrt(sq_sum /
               (static_cast<ValueType>(sz)));
    }
  return this->m_StandardDeviation;
}

/** Check validation of measurements*/
template< typename ValueType, typename MeanType >
bool
ResourceExpandedProbe< ValueType, MeanType >
::CheckValidation()
{
  if((this->m_NumberOfIteration == this->GetNumberOfStarts() )
     && (this->m_NumberOfIteration == this->GetNumberOfStops()))
    {
    return true;
    }
  return false;
}

/** Print System information */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
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
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::ExpandedReport(std::ostream & os, bool printSystemInfo, bool printReportHead )
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
     << std::setw( tabwide  ) << this->m_NumberOfThread
     << std::setw( tabwide  ) << this->GetTotal()
     << std::setw( tabwide  ) << this->GetMinimum()
     << std::setw( tabwide  ) << this->GetMean()
     << std::setw( tabwide  ) << this->GetMaximum()
     << std::setw( tabwide  ) << this->GetStandardDeviation();
  os << ss.str() << std::endl;
}

/** Print Probe Results. */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::AnalysisReport(std::ostream & os, bool printSystemInfo, bool printReportHead )
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
  ss << std::setw( namewide   ) << this->m_NameOfProbe
     << std::setw( tabwide    ) << this->m_NumberOfIteration
     << std::setw( tabwide    ) << this->m_NumberOfThread
     << std::setw( tabwide    ) << this->GetTotal()
     << std::setw( tabwide    ) << this->GetMinimum()
     << std::setw( tabwide    ) << this->GetMean() - this->GetMinimum()
     << std::setw( tabwide    ) << (this->GetMean()/this->GetMinimum())*100
     << std::setw( tabwide    ) << this->GetMean()
     << std::setw( tabwide    ) << this->GetMaximum() - this->GetMean()
     << std::setw( tabwide    ) << (this->GetMaximum()/this->GetMean())*-100
     << std::setw( tabwide    ) << this->GetMaximum()
     << std::setw( tabwide +5 ) << this->GetMaximum() - this->GetMinimum()
     << std::setw( tabwide    ) << this->GetStandardDeviation();
  os << ss.str() << std::endl;
}

/** Update the Min and Max values with an input value */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::UpdatekMinimumMaximumMeasuredValue(ValueType& value)
{
  if(this->GetNumberOfStops() == 0)
    {
    this->m_MinimumValue = value;
    this->m_MaximumValue = value;
    }
  else
    {
    this->m_MinimumValue = this->m_MinimumValue > value ? value : this->m_MinimumValue;
    this->m_MaximumValue = this->m_MaximumValue < value ? value : this->m_MaximumValue;
    }
}

/** Print Probe Results. */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
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
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
::PrintExpandedReportHead(std::ostream & os)
{
  std::stringstream ss;
  ss << std::setw( namewide   ) << "Name Of Probe"
     << std::setw( tabwide    ) << "Iteration"
     << std::setw( tabwide    ) << "# Threads"
     << std::setw( tabwide    ) << "Total(sec)"
     << std::setw( tabwide    ) << "Best(sec)"
     << std::setw( tabwide    ) << "Best(diff)"
     << std::setw( tabwide    ) << "Best(%)"
     << std::setw( tabwide    ) << "Mean(sec)"
     << std::setw( tabwide    ) << "Worst(diff)"
     << std::setw( tabwide    ) << "Worst(%)"
     << std::setw( tabwide    ) << "Worst(sec)"
     << std::setw( tabwide +5 ) << "Total Diff(sec)"
     << std::setw( tabwide    ) << "Std(sec)";

  os << ss.str() << std::endl;
}

/** Get System information */
template< typename ValueType, typename MeanType >
void
ResourceExpandedProbe< ValueType, MeanType >
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

#endif
