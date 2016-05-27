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

#include "itkResourceProbe.h"
#include "itkHighPriorityRealTimeClock.h"

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
 * \ingroup PerformanceBenchmarks
 *
 */
class PerformanceBenchmarks_EXPORT HighPriorityRealTimeProbe:public
  ResourceProbe< HighPriorityRealTimeClock::TimeStampType, HighPriorityRealTimeClock::TimeStampType >
{
public:

  /** Type for measuring time. See the RealTimeClock class for details on the
   * precision and units of this clock signal */
  typedef HighPriorityRealTimeClock::TimeStampType TimeStampType;

public:

  /** Constructor */
  HighPriorityRealTimeProbe();

  /** Destructor */
  virtual ~HighPriorityRealTimeProbe();

  /** Get the current time. */
  TimeStampType GetInstantValue() const;

  /** Get a handle to m_RealTimeClock. */
  itkGetConstObjectMacro( HighPriorityRealTimeClock, HighPriorityRealTimeClock );

private:
  HighPriorityRealTimeClock::Pointer m_HighPriorityRealTimeClock;

};
} // end namespace itk
#endif //itkHighPriorityRealTimeProbe_h
