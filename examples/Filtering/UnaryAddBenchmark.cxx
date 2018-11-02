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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAddImageFilter.h"

#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"
#include <fstream>
#include <cstdlib>

namespace
{
template<typename TImageType>
itk::SmartPointer<TImageType> ReadImage(const char *fname)
{

  using ReaderType = itk::ImageFileReader< TImageType >;
  auto reader = ReaderType::New();
  reader->SetFileName( fname );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    std::exit(EXIT_FAILURE);
    }
  return reader->GetOutput();


}
}

int main( int argc, char * argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile iterations threads input1ImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }
  const std::string timingsFileName = ReplaceOccurrence( argv[1], "__DATESTAMP__", PerfDateStamp());
  const int iterations = std::stoi( argv[2] );
  int threads = std::stoi( argv[3] );
  const char * inputImage1FileName = argv[4];
  const char * outputImageFileName = argv[5];

  if( threads > 0 )
    {
    MultiThreaderName::SetGlobalDefaultNumberOfThreads( threads );
    }

  constexpr unsigned int Dimension = 3;
  using PixelType = float;

  using ImageType = itk::Image< PixelType, Dimension >;

  ImageType::Pointer inputImage1 = ReadImage<ImageType>(inputImage1FileName);

  using FilterType = itk::AddImageFilter< ImageType, ImageType, ImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput1( inputImage1 );
  filter->SetInput2( 10 );

  itk::HighPriorityRealTimeProbesCollector collector;
  for( int ii = 0; ii < iterations; ++ii )
    {
    inputImage1->Modified();
    collector.Start("Add");
    filter->UpdateLargestPossibleRegion();
    collector.Stop("Add");
    }

  WriteExpandedReport(timingsFileName, collector, true, true, false);

  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
