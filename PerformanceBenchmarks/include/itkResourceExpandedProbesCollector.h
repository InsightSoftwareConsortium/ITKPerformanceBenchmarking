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
#ifndef itkResourceExpandedProbesCollector_h
#define itkResourceExpandedProbesCollector_h

#include "itkResourceExpandedProbe.h"
#include "itkResourceProbesCollectorBase.h"
#include "itkMemoryUsageObserver.h"

namespace itk
{
/** \class ResourceExpandedProbesCollectorBase
 *  \brief Aggregates a set of probes.
 *
 *  This class defines a set of ResourceProbes and assign names to them.
 *  The user can start and stop each one of the probes by addressing them by name.
 *
 *  \sa ResourceProbe
 *
 * \ingroup PerformanceBenchmarks
 */
template< typename TProbe >
class ResourceExpandedProbesCollector: public ResourceProbesCollectorBase<TProbe>
{
public:

 /** Type for counting how many times the probe has been started and stopped.
    */
  typedef unsigned long                                           CountType;
  typedef ResourceProbesCollectorBase<TProbe>                     SuperClass;
  typedef typename ResourceProbesCollectorBase<TProbe>::MapType   MapType;
  typedef typename ResourceProbesCollectorBase<TProbe>::IdType    IdType;

public:
  /** Constructor */
  ResourceExpandedProbesCollector();

  /** destructor */
  virtual ~ResourceExpandedProbesCollector();

  /** Start a probe with a particular name. If the time probe does not
   * exist, it will be created */
  virtual void Start(const char *name) ITK_OVERRIDE;

  /** Stop a time probe identified with a name */
  virtual void Stop(const char *name) ITK_OVERRIDE;

  /** Set name of target */
  void SetNameOfOverallProbe(const char* nameOfProbe);

  /** Set number of threads */
  void SetNumberOfThreads(const unsigned int numthreads);

  /** Print System information */
  void PrintSystemInformation(std::ostream & os = std::cout);

  /** Report the summary of results from the probes */
  virtual void Report(std::ostream & os = std::cout) ITK_OVERRIDE;

  /** Print all time probes' results. */
  void ExpandedReportAll(std::ostream & os = std::cout,
              bool printSystemInfo = true ,bool printReportHead = true);

  /** Print a specific time probe's results. */
  void ExpandedReport(const char* probeName,std::ostream & os =std::cout,
              bool printSystemInfo = true,bool printReportHead = true);

  /** Print all time probes' expanded results. */
  void AnalysisReportAll(std::ostream & os =std::cout,
                      bool printSystemInfo = true,bool printReportHead = true);

  /** Print a specific time probe's expanded results. */
  void AnalysisReport(const char*
                      probeName,std::ostream & os =std::cout,
                      bool printSystemInfo = true,bool printReportHead = true);

  /** Set enabling the funtion of UpdateProbeNameWithNumOfThreads */
  void EnableUpdatingProbeNameWithNumOfThreads(bool updateProbeName = true);

protected:
  /** Update a probe name with the number of threads. */
  std::string UpdateProbeNameWithNumOfThreads(std::string probeName);

  /** find a probe with a specific name. */
  TProbe& FindProbeWithName(const char* probeName);

  /** Print Probe Results. */
  void PrintExpandedReportHead(std::ostream & os =std::cout);

  /** Print Probe Results. */
  void PrintAnalysisReportHead(std::ostream & os =std::cout);

  /** Get System information */
  void GetSystemInformation();

protected:
  std::string                        m_NameOfProbe;
  std::string                        m_SystemName;
  std::string                        m_ProcessorName;
  int                                m_ProcessorCacheSize;
  float                              m_ProcessorClockFrequency;
  unsigned int                       m_NumberOfPhysicalCPU;
  unsigned int                       m_NumberOfLogicalCPU;
  unsigned int                       m_NumberOfAvailableCore;
  std::string                        m_OSName;
  std::string                        m_OSRelease;
  std::string                        m_OSVersion;
  std::string                        m_OSPlatform;
  bool                               m_Is64Bits;
  std::string                        m_ITKVersion;
  size_t                             m_TotalVirtualMemory;
  size_t                             m_AvailableVirtualMemory;
  size_t                             m_TotalPhysicalMemory;
  size_t                             m_AvailablePhysicalMemory;

  static const unsigned int          tabwide  = 15;
  static const unsigned int          namewide = 30;

  bool                               m_UpdateProbeNameWithNumOfThreads;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkResourceExpandedProbesCollector.hxx"
#endif

#endif //itkResourceProbesCollectorBase_h
