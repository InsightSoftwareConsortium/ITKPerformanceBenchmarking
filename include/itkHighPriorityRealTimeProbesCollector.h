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
#ifndef itkHighPriorityRealTimeProbesCollector_h
#define itkHighPriorityRealTimeProbesCollector_h

#include "LOCAL_itkResourceProbesCollectorBase.h"
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
 * \sa HighPriorityRealTimeClock
 * \ingroup PerformanceBenchmarking
 *
 */
class PerformanceBenchmarking_EXPORT HighPriorityRealTimeProbesCollector
  : public LOCAL_ResourceProbesCollectorBase<HighPriorityRealTimeProbe>
{
public:
  /** Constructor */
  HighPriorityRealTimeProbesCollector();

  /** Destructor */
  ~HighPriorityRealTimeProbesCollector() override;

private:
};
} // end namespace itk

#endif // itkHighPriorityRealTimeProbe_h
