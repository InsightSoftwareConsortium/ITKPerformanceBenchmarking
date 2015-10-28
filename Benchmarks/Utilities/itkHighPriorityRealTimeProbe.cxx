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


namespace itk
{
/** Constructor */
HighPriorityRealTimeProbe
::HighPriorityRealTimeProbe()
{
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
::SetNameOfTarget(std::string name)
{
  this->m_NameOfTargetClass = name;
}

/** Reset */
void
HighPriorityRealTimeProbe
::Reset()
{
  this->m_NameOfTargetClass.clear();
  this->m_TypeString.clear();
  this->m_UnitString.clear();

  this->ResetProbeData();
}

/** Reset */
void
HighPriorityRealTimeProbe
::ResetProbeData()
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
  this->UpdatekMinMaxValue(elapsedtime);
  this->m_TotalValue += elapsedtime;
  this->m_NumberOfStops++;
}

/** Compute mean, and standard deviation of measured computation time */
bool
HighPriorityRealTimeProbe
::Evaluate()
{
  this->m_NumberOfIteration = static_cast<CountType>(this->m_ElapsedTimeList.size());

  this->m_MeanValue         = m_TotalValue/static_cast<TimeStampType>(this->m_ElapsedTimeList.size());

  std::vector<TimeStampType> diff(this->m_ElapsedTimeList.size());
  std::transform(this->m_ElapsedTimeList.begin(),
                 this->m_ElapsedTimeList.end(),
                 diff.begin(),
                 std::bind2nd(std::minus<TimeStampType>(), this->m_MeanValue));
  TimeStampType sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  this->m_StandardDeviation = std::sqrt(sq_sum / (static_cast<TimeStampType>(this->m_ElapsedTimeList.size())-1));

  return this->CheckValidation();

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

/** Get Mean */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetMean() const
{
  return this->m_MeanValue;
}

/** Get Standard deviation */
HighPriorityRealTimeProbe::TimeStampType
HighPriorityRealTimeProbe
::GetStandardDeviation() const
{
  return this->m_StandardDeviation;
}

/** Print Probe Results. */
void
HighPriorityRealTimeProbe
::PrintProbeResults()
{
  std::cout << "Target Class:: "          << this->m_NameOfTargetClass << std::endl;
  std::cout << "- Number of Iteration     :: " << this->m_NumberOfIteration << std::endl;
  std::cout << "- Avg of computation time :: " << this->m_MeanValue << std::endl;
  std::cout << "- Min of computation time :: " << this->m_MinValue << std::endl;
  std::cout << "- Max of computation time :: " << this->m_MaxValue << std::endl;
  std::cout << "- Std of computation time :: " << this->m_StandardDeviation << std::endl;
}

void
HighPriorityRealTimeProbe
::TestPut(TimeStampType value)
{
  this->m_ElapsedTimeList.push_back(value);
  this->UpdatekMinMaxValue(value);
  this->m_TotalValue += value;

  this->m_NumberOfStarts++;
  this->m_NumberOfStops++;
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

} // end namespace itk
