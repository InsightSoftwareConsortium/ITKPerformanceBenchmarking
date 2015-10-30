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
#ifndef itkHighPriorityRealTimeProbe_h
#define itkHighPriorityRealTimeProbe_h

#include "itkConfigure.h"
#include "itkMacro.h"
#include "itkIntTypes.h"
#include <string>
#include "itkHighPriorityRealTimeClock.h"
#include "itksys/SystemInformation.hxx"

namespace itk
{
/** \class HighPriorityRealTimeProbe
 *
 *  \brief Computes the time passed between two points in code.
 *
 *   This class allows the user to trace the time passed between the execution
 *   of two pieces of code.  It can be started and stopped in order to evaluate
 *   the execution over multiple passes.  The values of time are taken from the
 *   HighPriorityRealTimeClock.
 *
 *   \sa HighPriorityRealTimeClock
 *
 */
class ITKCommon_EXPORT HighPriorityRealTimeProbe
{
public:

  /** Type for counting how many times the probe has been started and stopped.
    */
  typedef unsigned long CountType;

  /** Type for measuring time. See the RealTimeClock class for details on the
   * precision and units of this clock signal */
  typedef RealTimeClock::TimeStampType TimeStampType;

public:

  /** Constructor */
  HighPriorityRealTimeProbe();

  /** Destructor */
  virtual ~HighPriorityRealTimeProbe();

  /** Set name of target */
  void SetNameOfProbe(std::string name);

  /** Set number of threads */
  void SetMumberOfThreads(const unsigned int numthreads);

  /** Reset */
  void Reset();

  /** Returns the type probed value */
  std::string GetType() const;

  /** Returns the unit probed value */
  std::string GetUnit() const;

  /** Get the current time.
   *  Warning: the returned value is not the elapsed time since the last Start() call.
   */
  virtual TimeStampType GetInstantValue() const;

  /** Start counting the change of value */
  void Start();

  /** Stop counting the change of value.
   *
   * If a matching Start() has not been called before, there is no
   * effect.
   **/
  void Stop();

  /** Returns the number of times that the probe has been started */
  CountType GetNumberOfStarts() const;

  /** Returns the number of times that the probe has been stopped */
  CountType GetNumberOfStops() const;

  /** Returns the accumulated value changes between the starts and stops
   *  of the probe */
  virtual TimeStampType GetTotal() const;

  /** Returns the min value changes between the starts and stops
   *  of the probe */
  virtual TimeStampType GetMin() const;

  /** Returns the max value changes between the starts and stops
   *  of the probe */
  virtual TimeStampType GetMax() const;

  /** Returns the average value changes between the starts and stops
   *  of the probe. Evaluate() should be called prior to this function
   */
  virtual TimeStampType GetMean();

  /** Returns the standard deviation value changes between the starts and stops
   *  of the probe. Evaluate() should be called prior to this function
   */
  virtual TimeStampType GetStandardDeviation();

  /** Check validation of measurements*/
  bool CheckValidation();

  /** Print System information */
  void PrintSystemInformation(std::ostream & os = std::cout);

  /** Print Probe Results. */
  void Report(std::ostream & os =std::cout, bool printSystemInfo = true,
                   bool printReportHead = true);

  /** Print Probe Results. */
  void ExpandedReport(std::ostream & os =std::cout, bool printSystemInfo = true,
                           bool printReportHead = true);

  /** Get a handle to m_RealTimeClock. */
  itkGetConstObjectMacro( HighPriorityRealTimeClock, HighPriorityRealTimeClock );

protected:
  /** Update the Min and Max values with an input value */
  void UpdatekMinMaxValue(TimeStampType& value);

  /** Print Probe Results. */
  void PrintReportHead(std::ostream & os =std::cout);

  /** Print Probe Results. */
  void PrintExpandedReportHead(std::ostream & os =std::cout);

  /** Get System information */
  void GetSystemInformation();

private:
  std::string                        m_TypeString;
  std::string                        m_UnitString;
  HighPriorityRealTimeClock::Pointer m_HighPriorityRealTimeClock;

  std::string                        m_NameOfProbe;
  TimeStampType                      m_StartValue;
  TimeStampType                      m_TotalValue;
  TimeStampType                      m_MinValue;
  TimeStampType                      m_MaxValue;
  TimeStampType                      m_MeanValue;
  TimeStampType                      m_StandardDeviation;

  CountType                          m_NumberOfStarts;
  CountType                          m_NumberOfStops;
  CountType                          m_NumberOfIteration;

  std::vector<TimeStampType>         m_ElapsedTimeList;

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
