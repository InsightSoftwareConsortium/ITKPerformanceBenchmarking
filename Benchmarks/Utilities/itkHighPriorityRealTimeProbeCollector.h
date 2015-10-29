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
#ifndef HighPriorityRealTimeProbeCollector_h
#define HighPriorityRealTimeProbeCollector_h

#include "itkHighPriorityRealTimeProbe.h"
#include <map>

namespace itk
{
/** \class HighPriorityRealTimeProbeCollector
 *
 *  \brief Computes the multiple time passed between multiple pairs of
 *         two points in code.
 *
 *   This class allows the user to trace the multiple time passed between
 *   the execution of mutiple pair two pieces of code.
 *   It can be started and stopped with a name of Probe in order to evaluate
 *   the execution over multiple passes.  The values of time are taken from the
 *   HighPriorityRealTimeClock.
 *
 *   \sa HighPriorityRealTimeClock
 *
 */
class ITKCommon_EXPORT HighPriorityRealTimeProbeCollector
{
public:

  /** Type for counting how many times the probe has been started and stopped.
    */
  typedef unsigned long                                   CountType;

  /** Type for measuring time. See the RealTimeClock class for details on the
   * precision and units of this clock signal */
  typedef RealTimeClock::TimeStampType                    TimeStampType;

  /** Type for Probe */
  typedef HighPriorityRealTimeProbe                       ProbeType;

  /** Type for Probe List to control mutiple objects of HighPriorityRealTimeClock
   */
  typedef std::map<std::string,ProbeType>                 ProbeListType;

public:

  /** Constructor */
  HighPriorityRealTimeProbeCollector();

  /** Destructor */
  virtual ~HighPriorityRealTimeProbeCollector();

  /** Set name of target */
  void SetNameOfOverallProbe(std::string nameOfProbe);

  /** Set number of threads */
  void SetMumberOfThreads(const unsigned int numthreads);

  /** Reset */
  void Reset(std::string probeName);

  /** Get System information */
  void GetSystemInformation();

  /** Get Name of this class */
  const char * GetNameOfClass();

  /** Returns the type probed value */
  std::string GetType() const;

  /** Returns the unit probed value */
  std::string GetUnit() const;

  /** Get the current time.
   *  Warning: the returned value is not the elapsed time since the last Start() call.
   */
  virtual TimeStampType GetInstantValue() const;

  /** Start counting the change of value */
  void Start(std::string probeName);

  /** Stop counting the change of value.
   *
   * If a matching Start() has not been called before, there is no
   * effect.
   **/
  void Stop(std::string probeName);

  /** Returns the number of times that the probe has been started */
  CountType GetNumberOfStarts(std::string probeName);

  /** Returns the number of times that the probe has been stopped */
  CountType GetNumberOfStops(std::string probeName);

  /** Returns the accumulated value changes between the starts and stops
   *  of the probe */
  virtual TimeStampType GetTotal(std::string probeName);

  /** Returns the min value changes between the starts and stops
   *  of the probe */
  virtual TimeStampType GetMin(std::string probeName);

  /** Returns the max value changes between the starts and stops
   *  of the probe */
  virtual TimeStampType GetMax(std::string probeName);

  /** Returns the average value changes between the starts and stops
   *  of the probe. Evaluate() should be called prior to this function
   */
  virtual TimeStampType GetMean(std::string probeName);

  /** Returns the standard deviation value changes between the starts and stops
   *  of the probe. Evaluate() should be called prior to this function
   */
  virtual TimeStampType GetStandardDeviation(std::string probeName);

  /** Check validation of measurements*/
  bool CheckValidation(std::string probeName);

  /** Print System information */
  void PrintSystemInformation(std::ostream & os = std::cout);

  /** Print Probe Results. */
  void PrintReportHead(std::ostream & os =std::cout);

  /** Print Probe Results. */
  void PrintReport(std::string probeName,std::ostream & os =std::cout,
                   bool printSystemInfo = true,bool printReportHead = true);

  /** Print Probe Results. */
  void PrintExpandedReportHead(std::ostream & os =std::cout);

  /** Print Probe Results. */
  void PrintExpandedReport(std::string probeName,std::ostream & os =std::cout,
                           bool printSystemInfo = true,bool printReportHead = true);

  /** Get a handle to m_RealTimeClock. */
  itkGetConstObjectMacro( HighPriorityRealTimeClock, HighPriorityRealTimeClock );

protected:
  HighPriorityRealTimeProbeCollector::ProbeType&
     FindHighPriorityRealTimeProbeWithName(std::string & probeName);

  bool CheckHighPriorityRealTimeProbeWithName(std::string & probeName,
                                              ProbeType& probe);

private:
  std::string                        m_TypeString;
  std::string                        m_UnitString;
  HighPriorityRealTimeClock::Pointer m_HighPriorityRealTimeClock;

  std::string                        m_NameOfProbe;
  ProbeListType                      m_ProbeList;

  itksys::SystemInformation          m_SystemInformation;
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

};
} // end namespace itk

#endif //itkHighPriorityRealTimeProbe_h
