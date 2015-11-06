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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef itkResourceProbe_hxx
#define itkResourceProbe_hxx

#include "itkResourceProbe2.h"
#include "itkNumericTraits.h"

namespace itk
{
/** Constructor */
template< typename ValueType, typename MeanType >
ResourceProbe2< ValueType, MeanType >
::ResourceProbe2(const std::string & type, const std::string & unit):
  m_TypeString(type), m_UnitString(unit)
{
  this->Reset();
}

/** Destructor */
template< typename ValueType, typename MeanType >
ResourceProbe2< ValueType, MeanType >
::~ResourceProbe2()
{}

/** Reset */
template< typename ValueType, typename MeanType >
void
ResourceProbe2< ValueType, MeanType >
::Reset(void)
{
  this->m_TotalValue      = NumericTraits< ValueType >::ZeroValue();
  this->m_StartValue      = NumericTraits< ValueType >::ZeroValue();
  this->m_NumberOfStarts  = NumericTraits< CountType >::ZeroValue();
  this->m_NumberOfStops   = NumericTraits< CountType >::ZeroValue();
}

/** Returns the type probed value */
template< typename ValueType, typename MeanType >
std::string
ResourceProbe2< ValueType, MeanType >
::GetType(void) const
{
  return this->m_TypeString;
}

/** Returns the unit probed value */
template< typename ValueType, typename MeanType >
std::string
ResourceProbe2< ValueType, MeanType >
::GetUnit(void) const
{
  return this->m_UnitString;
}

/** Start counting */
template< typename ValueType, typename MeanType >
void
ResourceProbe2< ValueType, MeanType >
::Start(void)
{
  this->m_NumberOfStarts++;
  this->m_StartValue = this->GetInstantValue();
}


/** Stop the probe */
template< typename ValueType, typename MeanType >
void
ResourceProbe2< ValueType, MeanType >
::Stop(void)
{
  if ( this->m_NumberOfStops == this->m_NumberOfStarts )
    {
    return;
    }
  this->m_TotalValue += this->GetInstantValue() - this->m_StartValue;
  this->m_NumberOfStops++;
}

  /** Return measured value that the probe started */
template< typename ValueType, typename MeanType >
ValueType
ResourceProbe2< ValueType, MeanType >
::GetStartValue() const
{
  return this->m_StartValue;
}

/** Get Number of Starts */
template< typename ValueType, typename MeanType >
typename ResourceProbe2< ValueType, MeanType >::CountType
ResourceProbe2< ValueType, MeanType >
::GetNumberOfStarts(void) const
{
  return this->m_NumberOfStarts;
}

/** Get Number of Stops */
template< typename ValueType, typename MeanType >
typename ResourceProbe2< ValueType, MeanType >::CountType
ResourceProbe2< ValueType, MeanType >
::GetNumberOfStops(void) const
{
  return this->m_NumberOfStops;
}

/** Increase  the number of starts*/
template< typename ValueType, typename MeanType >
void
ResourceProbe2< ValueType, MeanType >
::IncreaseNumberOfStarts(CountType val)
{
  this->m_NumberOfStarts += val;
}

/** Increase  the number of starts*/
template< typename ValueType, typename MeanType >
void
ResourceProbe2< ValueType, MeanType >
::IncreaseNumberOfStops(CountType val)
{
  this->m_NumberOfStops += val;
}

/** Get Total */
template< typename ValueType, typename MeanType >
ValueType
ResourceProbe2< ValueType, MeanType >
::GetTotal(void) const
{
  return this->m_TotalValue;
}

/** Set Start */
template< typename ValueType, typename MeanType >
void
ResourceProbe2< ValueType, MeanType >
::SetStartValue(const ValueType value)
{
  this->m_StartValue = value;
}

/** Set Total */
template< typename ValueType, typename MeanType >
void
ResourceProbe2< ValueType, MeanType >
::SetTotalValue(const ValueType value)
{
  this->m_TotalValue = value;
}

/** Get Mean */
template< typename ValueType, typename MeanType >
MeanType
ResourceProbe2< ValueType, MeanType >
::GetMean(void) const
{
  MeanType meanValue = NumericTraits< MeanType >::ZeroValue();

  if ( this->m_NumberOfStops )
    {
    meanValue = static_cast< MeanType >( this->m_TotalValue ) / static_cast< MeanType >( this->m_NumberOfStops );
    }

  return meanValue;
}
} // end namespace itk

#endif
