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
#ifndef itkResourceExpandedProbe_h
#define itkResourceExpandedProbe_h

//#include "itkConfigure.h"
#include "itkResourceProbe2.h"
#include <vector>

namespace itk
{
/** \class ResourceExpandedProbe
 *
 *  \brief Computes the change of a value between two points in code.
  *
  *   This class generates a expanded report (including system information, minimum
  *   maximun, mean and standard deviation) of all the probes (time, memory, etc.)
  *   between the execution of two pieces of code. It can be started and
  *   stopped in order to evaluate the execution over multiple passes.
 *
 *   \sa HighPriorityRealTimeClock
 *
 */
template< typename ValueType, typename MeanType >
class ResourceExpandedProbe:public ResourceProbe2< ValueType, MeanType >
{
public:
    /** Type for counting how many times the probe has been started and stopped.
      */
    typedef typename ResourceProbe2< ValueType, MeanType >::CountType  CountType;

    typedef ResourceProbe2< ValueType, MeanType >                      SuperClass;

public:

  /** Constructor */
  ResourceExpandedProbe(const std::string & type, const std::string & unit);

  /** Destructor */
  virtual ~ResourceExpandedProbe();

  /** Set name of target */
  void SetNameOfProbe(const char* name);

  /** Set number of threads */
  void SetNumberOfThreads(const unsigned int numthreads);

  /** Reset */
  virtual void Reset() ITK_OVERRIDE;

  /** Start counting the change of value */
  virtual void Start() ITK_OVERRIDE;

  /** Stop counting the change of value.
   *
   * If a matching Start() has not been called before, there is no
   * effect.
   **/
  virtual void Stop() ITK_OVERRIDE;

  /** Returns the min value changes between the starts and stops
   *  of the probe */
  virtual ValueType GetMinimum() const;

  /** Returns the max value changes between the starts and stops
   *  of the probe */
  virtual ValueType GetMaximum() const;

  /** Returns the average value changes between the starts and stops
   *  of the probe. */
  virtual ValueType GetMean() const;

  /** Returns the standard deviation value changes between the starts and stops
   *  of the probe. */
  virtual ValueType GetStandardDeviation();

  /** Check validation of probe*/
  bool CheckValidation();

  /** Print System information */
  void PrintSystemInformation(std::ostream & os = std::cout);

  /** Print Probe Results. */
  void ExpandedReport(std::ostream & os =std::cout, bool printSystemInfo = true,
              bool printReportHead = true);

  /** Print Probe Results. */
  void AnalysisReport(std::ostream & os =std::cout, bool printSystemInfo = true,
                      bool printReportHead = true);

protected:
  /** Update the Min and Max values with an input value */
  void UpdatekMinimumMaximumMeasuredValue(ValueType& value);

  /** Print Probe Results. */
  void PrintReportHead(std::ostream & os =std::cout);

  /** Print Probe Results. */
  void PrintExpandedReportHead(std::ostream & os =std::cout);

  /** Get System information */
  void GetSystemInformation();

private:

  std::string                    m_NameOfProbe;
  ValueType                      m_MinimumValue;
  ValueType                      m_MaximumValue;
  ValueType                      m_MeanValue;
  ValueType                      m_StandardDeviation;

  CountType                      m_NumberOfIteration;
  CountType                      m_NumberOfThread;

  std::vector<ValueType>         m_ProbeValueList;

  std::string                    m_SystemName;
  std::string                    m_ProcessorName;
  int                            m_ProcessorCacheSize;
  float                          m_ProcessorClockFrequency;
  unsigned int                   m_NumberOfPhysicalCPU;
  unsigned int                   m_NumberOfLogicalCPU;
  unsigned int                   m_NumberOfAvailableCore;
  std::string                    m_OSName;
  std::string                    m_OSRelease;
  std::string                    m_OSVersion;
  std::string                    m_OSPlatform;
  bool                           m_Is64Bits;
  std::string                    m_ITKVersion;
  size_t                         m_TotalVirtualMemory;
  size_t                         m_AvailableVirtualMemory;
  size_t                         m_TotalPhysicalMemory;
  size_t                         m_AvailablePhysicalMemory;

  static const unsigned int      tabwide  = 15;
  static const unsigned int      namewide = 30;

};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkResourceExpandedProbe.hxx"
#endif

#endif //itkResourceExpandedProbe_h
