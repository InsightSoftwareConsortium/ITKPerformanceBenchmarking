/*=========================================================================
 *
 *  Copyright NumFOCUS
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
#include <iostream>
#include "itkHighPriorityRealTimeClock.h"

#if defined(_WIN32)

#else

#  include <sys/time.h>
#  include <sys/resource.h>
#  include <cerrno>

#endif // defined(WIN32) || defined(_WIN32)

namespace itk
{

void
HighPriorityRealTimeClock ::DisplayErrorMessage()
{
#if defined(_WIN32)
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR)&lpMsgBuf,
                0,
                NULL);
  MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
  LocalFree(lpMsgBuf);
#endif // defined(_WIN32)
}


HighPriorityRealTimeClock ::HighPriorityRealTimeClock()
{
  this->RaisePriority();
}


HighPriorityRealTimeClock ::~HighPriorityRealTimeClock()
{
  this->RestorePriority();
}


void
HighPriorityRealTimeClock ::RaisePriority()
{
#if defined(_WIN32)
  this->m_OldPriorityClass = ::GetPriorityClass(::GetCurrentProcess());
  if (!this->m_OldPriorityClass)
  {
    this->DisplayErrorMessage();
    itkExceptionMacro("Current priority class could not be retrieved");
  }
  // REALTIME_PRIORITY_CLASS will pretty much block the mouse, cause
  // some program to lose socket connection, etc.
  else if (!::SetPriorityClass(::GetCurrentProcess(), HIGH_PRIORITY_CLASS))
  {
    // Note that Administrators will be able to reach REALTIME, whether other
    // users will be capped to HIGH (without error).
    this->DisplayErrorMessage();
    itkExceptionMacro("Priority class could not be set");
  }

  this->m_OldThreadPriority = ::GetThreadPriority(::GetCurrentThread());
  if (this->m_OldThreadPriority == THREAD_PRIORITY_ERROR_RETURN)
  {
    this->DisplayErrorMessage();
    itkExceptionMacro("Current thread priority could not be retrieved");
  }
  else if (!::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
  {
    this->DisplayErrorMessage();
    itkExceptionMacro("Thread priority could not be set");
  }
#else
  errno = 0;
  this->m_OldProcessPriority = getpriority(PRIO_PROCESS, 0);
  if (this->m_OldProcessPriority == -1 && errno != 0)
  {
    itkExceptionMacro("Current process priority could not be retrieved");
  }
  else
  {
    errno = 0;
    // Technically only root can upgrade our priority, i.e. set the priority
    // to such a low value that the system will favor our process for
    // scheduling. In most other cases, we would get an EACCESS.
    int prio;
#  ifdef __APPLE__
    prio = -20;
#  else
    prio = -NZERO;
#  endif
    if (setpriority(PRIO_PROCESS, 0, prio) == -1 && errno != EACCES && errno != EPERM)
    {
      std::string msg("Process priority could not be set: ");
      switch (errno)
      {
        case EACCES:
          msg += "EACCES";
          break;
        case EINVAL:
          msg += "EINVAL";
          break;
        case ENOTSUP:
          msg += "ENOTSUP";
          break;
        case EPERM:
          msg += "EPERM";
          break;
        case ESRCH:
          msg += "ESRCH";
          break;
        default:
          msg += "Unknown";
          break;
      }
      itkExceptionMacro(<< msg.c_str());
    }
  }
#endif // defined(_WIN32)
}


void
HighPriorityRealTimeClock ::RestorePriority()
{
#if defined(_WIN32)
  if (this->m_OldPriorityClass && !::SetPriorityClass(::GetCurrentProcess(), this->m_OldPriorityClass))
  {
    this->DisplayErrorMessage();
  }

  if (this->m_OldThreadPriority != THREAD_PRIORITY_ERROR_RETURN &&
      !::SetThreadPriority(::GetCurrentThread(), this->m_OldThreadPriority))
  {
    this->DisplayErrorMessage();
  }
#else
  errno = 0;
  if (setpriority(PRIO_PROCESS, 0, this->m_OldProcessPriority) == -1)
  {
    std::string msg("Process priority could not be set: ");
    switch (errno)
    {
      case EACCES:
        msg += "EACCES";
        break;
      case EINVAL:
        msg += "EINVAL";
        break;
      case ENOTSUP:
        msg += "ENOTSUP";
        break;
      case EPERM:
        msg += "EPERM";
        break;
      case ESRCH:
        msg += "ESRCH";
        break;
      default:
        msg += "Unknown";
        break;
    }
    itkExceptionMacro(<< msg.c_str());
  }
#endif // defined(_WIN32)
}

} // namespace itk
