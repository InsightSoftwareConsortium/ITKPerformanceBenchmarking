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
#include "PerformanceBenchmarkingInformation.h"
#include "PerformanceBenchmarkingUtilities.h"
#include <itksys/SystemTools.hxx>
#include <cstdlib>
#include <ostream>
#include <fstream>

/**  Decorate with json from an environmental variable
 *
 * export ITKPERFORMANCEBENCHMARK_AUX_JSON="{ \"ITK_PROGRAMMERS_ARE\": [ \"Spectacular\", \"Awesome\", \"Brilliant\", \"Good Looking\" ] }"
 *
 * For ITK versions without the new itk::BuildInformation
cd ${ITK_SRC}

export GIT_CONFIG_SHA1="$(git rev-parse HEAD)"
export GIT_CONFIG_DATE="$(git show -s --format=%ci HEAD)"
export GIT_LOCAL_MODIFICATIONS="$(git  diff --shortstat HEAD)"
export ITKPERFORMANCEBENCHMARK_AUX_JSON="
{
\"ITK_MANUAL_BUILD_INFO\": {
 \"GIT_CONFIG_DATE\": \"${GIT_CONFIG_DATE}\",
 \"GIT_CONFIG_SHA1\": \"${GIT_CONFIG_SHA1}\",
 \"GIT_LOCAL_MODIFICATIONS\": \"${GIT_LOCAL_MODIFICATIONS}\"
}
}
"
echo ${ITKPERFORMANCEBENCHMARK_AUX_JSON}
 */
static std::string getEnvJsonMap()
{
  char * auxEnvironmentJson  =  getenv( "ITKPERFORMANCEBENCHMARK_AUX_JSON" );
  if ( auxEnvironmentJson != nullptr )
  {
    jsonxx::Object auxEnvironmentObject;
    auxEnvironmentObject.parse(auxEnvironmentJson);
    return auxEnvironmentObject.json();
  }
  return "{}";
}


static std::string PerformanceGuessGitHash()
{
  std::string sha1Guess("HASHNOTEXPOSED");

  jsonxx::Object auxEnvironmentObject;
  auxEnvironmentObject.parse(getEnvJsonMap());
  if( auxEnvironmentObject.has<jsonxx::Object>("ITK_MANUAL_BUILD_INFO") )
    {
    if (auxEnvironmentObject.get<jsonxx::Object>("ITK_MANUAL_BUILD_INFO").has<jsonxx::String>("GIT_CONFIG_SHA1"))
      {
      sha1Guess =
          auxEnvironmentObject.get<jsonxx::Object>("ITK_MANUAL_BUILD_INFO") \
                              .get<jsonxx::String>("GIT_CONFIG_SHA1", sha1Guess) + "_ENV";
      }
  }

#ifdef ITK_HAS_INFORMATION_H
  const std::string itkHash = itk::BuildInformation::GetInstance()->GetValue("GIT_CONFIG_SHA1");
  if( itkHash.size() > 1 )
  {
    sha1Guess = itkHash;
  }
#endif
  return sha1Guess;
}

std::string PerfDateStamp()
{
  std::time_t t = std::time(nullptr);
  char mbstr[100];
  if (std::strftime(mbstr, sizeof(mbstr), "%F-%T", std::localtime(&t))) {
    return mbstr;
  }
  return "";
}

std::string ReplaceOccurrence( std::string str,
                              const  std::string && findvalue,
                              const  std::string && replacevalue)
{
  /* Locate the substring to replace. */
  auto index = str.find(findvalue);
  if (index == std::string::npos)
  {
    return str;
  }
  static const std::string under{"_"};
  /* Make the replacement. */
  str.replace(index, findvalue.size(), under + replacevalue + under + PerformanceGuessGitHash() );
  return str;
}

void WriteExpandedReport(const std::string &timingsFileName, itk::HighPriorityRealTimeProbesCollector &collector,
                         bool printSystemInfo, bool printReportHead, bool useTabs)
{
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );
  std::ofstream timingsFile(timingsFileName, std::ios_base::out);
  if(timingsFileName.find(".json"))
  {
    std::stringstream probejsonstream;
    collector.JSONReport(probejsonstream, printSystemInfo);
    const std::string finalJsonString = DecorateWithBuildInformation(probejsonstream.str());
    timingsFile << finalJsonString;
  }
  else {
    printSystemInfo = false;
    printReportHead = true;
    useTabs = true;
    collector.ExpandedReport(timingsFile, printSystemInfo, printReportHead, useTabs);\
  }
  timingsFile.close();
}

std::string DecorateWithBuildInformation( std::string  inputJson)
{
  jsonxx::Object o;
  o.parse(inputJson);
#ifdef ITK_HAS_INFORMATION_H
  {
    jsonxx::Object itkInfoJsonObject;
    for (auto items : itk::BuildInformation::GetInstance()->GetMap())
    {
      itkInfoJsonObject << items.first << items.second.m_Value;
      itkInfoJsonObject << items.first+"_description" << items.second.m_Description;
    }
    o << "ITKBuildInformation" << itkInfoJsonObject;
  }
#endif
  {
    jsonxx::Object itkPerformanceBenchmarkJsonObject;

    for (auto items : itk::PerformanceBenchmarkingInformation::GetInstance()->GetMap())
    {
      itkPerformanceBenchmarkJsonObject << items.first << items.second.m_Value;
      itkPerformanceBenchmarkJsonObject << items.first+"_description" << items.second.m_Description;
    }
    o << "PerformanceBenchmarkInformation" << itkPerformanceBenchmarkJsonObject;
  }
  {
    jsonxx::Object runTimeEnvJsonObject;
    const unsigned int defaultNumberOfThreads = itk::MultiThreader::GetGlobalDefaultNumberOfThreads();
    runTimeEnvJsonObject << "GetGlobalDefaultNumberOfThreads" << defaultNumberOfThreads;
    //NOTE: This is the load average, that includes this test, and many other test, and what the
    //      OS was doing around the time of the test.  It is not terribly reliable, but if it is
    //      much higher than the max number of CPU's then the tests are going to be very unreliable.
    itksys::SystemInformation hardwareInfo;
    const double loadAverage = hardwareInfo.GetLoadAverage();
    runTimeEnvJsonObject << "ReportWritingLoadAverage" << loadAverage;
    o << "RunTimeInformation" << runTimeEnvJsonObject;
  }
  {
      jsonxx::Object auxEnvironmentObject;
      auxEnvironmentObject.parse(getEnvJsonMap());
      // We need the contents of the env json, not the whole json
      const jsonxx::Object::container internalMap = auxEnvironmentObject.kv_map();
      for(const auto internalElement : internalMap)
      {
        o << internalElement.first << *(internalElement.second);
      }
  }
  //std::cout << o.json() << std::endl;
  return o.json();
}
