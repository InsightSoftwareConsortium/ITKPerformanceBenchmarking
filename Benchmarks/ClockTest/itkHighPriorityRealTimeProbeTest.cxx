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

#include <iostream>
#include "itkHighPriorityRealTimeProbe.h"
#include "itkMath.h"

int main( int, char * [] )
{
  // Create an ITK time probe
  itk::HighPriorityRealTimeProbe localTimer;
  localTimer.SetNameOfProbe("Simple for loop");
  // Print the initial values
  std::cout << "Testing itk::HighPriorityRealTimeProbe"             << std::endl;
  std::cout << "Type:           " << localTimer.GetType()           << std::endl;
  std::cout << "Unit:           " << localTimer.GetUnit()           << std::endl;

  unsigned int numOfThread(1);
  unsigned int max_numOfThread(20);
  unsigned int iteration (100);

  for(numOfThread =1; numOfThread <= max_numOfThread; ++numOfThread)
  {
    localTimer.SetMumberOfThreads(numOfThread);
    for(int i=0; i<10; ++i)
    {
      // time a task
      localTimer.Start();
      double sum = 0.0;
      for( unsigned int i = 0; i < 1e6; ++i )
      {
        sum += i;
       }
       localTimer.Stop();
    }

    if(numOfThread ==1)
      localTimer.Report();
    else
      localTimer.Report(std::cout, false, false);

    localTimer.Reset();
  }

  return EXIT_SUCCESS;
}
