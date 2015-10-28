/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHighPriorityRealTimeClock.h,v $
  Language:  C++
  Date:      $Date: 2007/04/04 19:57:04 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkHighPriorityRealTimeClock_h
#define __itkHighPriorityRealTimeClock_h

#include <itkRealTimeClock.h>

#if defined(WIN32) || defined(_WIN32)

#include <windows.h>

#else

#endif  // defined(WIN32) || defined(_WIN32)

namespace itk
{

/** \class HighPriorityRealTimeClock
* \brief The HighPriorityRealTimeClock provides a timestamp from a real-time clock. It subclasses from RealTimeClock to bump the thread priority and affinity.
*
*/

class ITK_EXPORT HighPriorityRealTimeClock : public RealTimeClock
{
public:
  typedef HighPriorityRealTimeClock Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for defining the name of the class */
  itkTypeMacro(HighPriorityRealTimeClock, Object);

  /** Method for creation through the object factory */
  itkNewMacro(Self);

protected:

  /** constructor */
  HighPriorityRealTimeClock();

  /** destructor */
  virtual ~HighPriorityRealTimeClock();

  /** Method for raising and restoring the priority */
  virtual void RaisePriority();
  virtual void RestorePriority();

private:

#if defined(WIN32) || defined(_WIN32)

  DWORD dwOldPriorityClass;
  int nOldThreadPriority;

#else

  int OldProcessPriority;

#endif  // defined(WIN32) || defined(_WIN32)

  void DisplayErrorMessage();
};

} // end of namespace itk

#endif  // __itkHighPriorityRealTimeClock_h
