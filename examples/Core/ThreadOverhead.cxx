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

#include "itkImage.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkTimeProbesCollectorBase.h"
#include "itkHighPriorityRealTimeProbe.h"
#include "itkHighPriorityRealTimeProbesCollector.h"

#include <sstream>
#include <fstream>

// This benchmark estimate the overhead for using an additional thread
// in an ImageFilter a.k.a the time it takes to "spawn" a thread.
//
// The overhead for spawning threads is computed by measuring the
// time it takes an Functor Filter to run with 1 thread on 1 pixel,
// and the time it takes to run with N threads on N pixels. Each
// thread does the one pixel trivial operation. The difference in
// execution time is considered the overhead for spawning the
// threads. Dividing by the number of additional threads gives us the
// overhead cost of “spawning” or dispatching a single thread.


typedef itk::HighPriorityRealTimeProbe    ProbeType;
typedef itk::HighPriorityRealTimeProbesCollector CollectorType;

//typedef itk::TimeProbesCollectorBase CollectorType;
//typedef itk::TimeProbe               ProbeType;


namespace
{

CollectorType collector;

template< typename TInput, typename TOutput >
class Op
{
public:
  Op() {}
  virtual ~Op() {}
  bool operator!=(const Op &) const
  {
    return false;
  }

  bool operator==(const Op & other) const
  {
    return !( *this != other );
  }

  inline TOutput operator()(const TInput & A) const
  {
    return static_cast< TOutput >( A + 1);
  }
};
}

ProbeType time_it(unsigned int threads, unsigned int iterations, bool realtime = true)
{

  const unsigned int Dimension = 1;
  typedef float PixelType;

  typedef itk::Image<PixelType,Dimension> ImageType;

  ImageType::Pointer image = ImageType::New();

  ImageType::SizeType imageSize = { threads };
  image->SetRegions(ImageType::RegionType(imageSize));
  image->Allocate();
  image->FillBuffer(0);


  typedef itk::UnaryFunctorImageFilter< ImageType,
                                        ImageType,
                                        Op<PixelType,PixelType > > FilterType;

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(image);
  filter->SetNumberOfThreads( threads );

  // execute one time out of the loop to allocate memory
  filter->UpdateLargestPossibleRegion();

  std::ostringstream ss;
  ss << "FilterWithThreads-" << threads;

  const std::string name = ss.str();

  for( int ii = 0; ii < iterations; ++ii )
    {
    image->Modified();
    collector.Start(name.c_str());
    filter->UpdateLargestPossibleRegion();
    collector.Stop(name.c_str());
    }

  return collector.GetProbe(name.c_str());
}


int main( int argc, char * argv[] )
{
  if( argc > 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile [iterations [threads]]" << std::endl;
    return EXIT_FAILURE;
    }

  const char * timingsFileName = argv[1];
  const int iterations = (argc>2) ? atoi( argv[2] ): 500;
  const int threads = (argc>3) ? atoi( argv[3] ) : itk::MultiThreaderBase::GetGlobalDefaultNumberOfThreads();

  if (threads == 1)
    {
    std::cout << "Unable to estimate the cost with only one thread!" << std::endl;
    return EXIT_FAILURE;
    }

  ProbeType t1 = time_it(1,iterations);

  ProbeType t2 = time_it(threads,iterations);


  bool printReportHead = true;
  bool printSystemInfo = true;
  bool useTabs = false;
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );

  std::ofstream timingsFile( timingsFileName, std::ios::out );
  printSystemInfo = false;
  useTabs = true;
  collector.ExpandedReport( timingsFile, printSystemInfo, printReportHead, useTabs );

  double cost = (t2.GetMinimum() - t1.GetMinimum())/(threads-1.0);

  std::cout << "\n\nEstimated overhead cost per thread: " << cost * 1e6 << " micro-seconds\n\n";

  return EXIT_SUCCESS;
}
