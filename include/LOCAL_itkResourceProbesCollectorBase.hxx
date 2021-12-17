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
#ifndef itkLOCALResourceProbesCollectorBase_hxx
#define itkLOCALResourceProbesCollectorBase_hxx

#include <iostream>

namespace itk
{

template <typename TProbe>
LOCAL_ResourceProbesCollectorBase<TProbe>::~LOCAL_ResourceProbesCollectorBase() = default;


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::Start(const char * id)
{
  // if the probe does not exist yet, it is created.
  this->m_Probes[id].SetNameOfProbe(id);
  this->m_Probes[id].Start();
}


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::Stop(const char * id)
{
  IdType tid = id;

  auto pos = this->m_Probes.find(tid);
  if (pos == this->m_Probes.end())
  {
    itkGenericExceptionMacro(<< "The probe \"" << id << "\" does not exist. It can not be stopped.");
    return;
  }
  pos->second.Stop();
}


template <typename TProbe>
const TProbe &
LOCAL_ResourceProbesCollectorBase<TProbe>::GetProbe(const char * id) const
{
  IdType tid = id;

  auto pos = this->m_Probes.find(tid);
  if (pos == this->m_Probes.end())
  {
    itkGenericExceptionMacro(<< "The probe \"" << id << "\" does not exist.");
  }
  return pos->second;
}


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::Report(std::ostream & os,
                                                  bool           printSystemInfo,
                                                  bool           printReportHead,
                                                  bool           useTabs)
{
  auto                             probe = this->m_Probes.begin();
  typename MapType::const_iterator end = this->m_Probes.end();

  if (probe == end)
  {
    os << "No probes have been created" << std::endl;
    return;
  }

  bool firstProbe = true;
  while (probe != end)
  {
    if (firstProbe)
    {
      probe->second.Report(os, printSystemInfo, printReportHead, useTabs);
      firstProbe = false;
    }
    else
    {
      probe->second.Report(os, false, false, useTabs);
    }

    ++probe;
  }
}


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::Report(const char *   name,
                                                  std::ostream & os,
                                                  bool           printSystemInfo,
                                                  bool           printReportHead,
                                                  bool           useTabs)
{
  const IdType tid = name;

  auto pos = this->m_Probes.find(tid);
  if (pos == this->m_Probes.end())
  {
    os << "The probe \"" << name << "\" does not exist. It's report is not available" << std::endl;
    return;
  }

  pos->second.Report(os, printSystemInfo, printReportHead, useTabs);
}


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::ExpandedReport(std::ostream & os,
                                                          bool           printSystemInfo,
                                                          bool           printReportHead,
                                                          bool           useTabs)
{
  auto                             probe = this->m_Probes.begin();
  typename MapType::const_iterator end = this->m_Probes.end();

  if (probe == end)
  {
    os << "No probes have been created" << std::endl;
    return;
  }

  bool firstProbe = true;
  while (probe != end)
  {
    if (firstProbe)
    {
      probe->second.ExpandedReport(os, printSystemInfo, printReportHead, useTabs);
      firstProbe = false;
    }
    else
    {
      probe->second.ExpandedReport(os, false, false, useTabs);
    }

    ++probe;
  }
}


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::ExpandedReport(const char *   name,
                                                          std::ostream & os,
                                                          bool           printSystemInfo,
                                                          bool           printReportHead,
                                                          bool           useTabs)
{
  const IdType tid = name;

  auto pos = this->m_Probes.find(tid);
  if (pos == this->m_Probes.end())
  {
    os << "The probe \"" << name << "\" does not exist. It's report is not available" << std::endl;
    return;
  }

  pos->second.ExpandedReport(os, printSystemInfo, printReportHead, useTabs);
}


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::JSONReport(std::ostream & os, bool printSystemInfo)
{
  auto                             probe = this->m_Probes.begin();
  typename MapType::const_iterator end = this->m_Probes.end();

  if (probe == end)
  {
    os << R"({ "Status": "No probes have been created" })" << std::endl;
    return;
  }

  os << "{\n";
  if (printSystemInfo)
  {
    os << "  \"SystemInformation\": ";
    probe->second.PrintJSONSystemInformation(os);
    os << ",\n";
  }
  os << "  \"Probes\": [\n";
  bool firstProbe = true;
  while (probe != end)
  {
    if (firstProbe)
    {
      probe->second.JSONReport(os);
      firstProbe = false;
    }
    else
    {
      os << ",\n";
      probe->second.JSONReport(os);
    }

    ++probe;
  }
  os << "\n  ]\n}" << std::endl;
}


template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::JSONReport(const char * name, std::ostream & os)
{
  const IdType tid = name;

  auto pos = this->m_Probes.find(tid);
  if (pos == this->m_Probes.end())
  {
    os << R"(  { "ProbeName": ")" << name << R"(", "Status": "Does not exist!" })" << std::endl;
    return;
  }

  pos->second.JSONReport(os);
}

template <typename TProbe>
void
LOCAL_ResourceProbesCollectorBase<TProbe>::Clear()
{
  this->m_Probes.clear();
}


} // end namespace itk

#endif // itkLOCALResourceProbesCollectorBase_hxx
