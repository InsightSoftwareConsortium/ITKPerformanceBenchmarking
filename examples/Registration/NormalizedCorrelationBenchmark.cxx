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
#include "itkFFTNormalizedCorrelationImageFilter.h"
#include "itkFFTPadImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

#include "itkHighPriorityRealTimeProbesCollector.h"

#include <fstream>


int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile threads fixedImageFile movingImageFile" << std::endl;
    return EXIT_FAILURE;
    }
  const char * timingsFileName = argv[1];
  int threads = atoi( argv[2] );
  const char * fixedImageFileName = argv[3];
  const char * movingImageFileName = argv[4];

  if( threads > 0 )
    {
    itk::MultiThreader::SetGlobalDefaultNumberOfThreads( threads );
    }

  const unsigned int Dimension = 3;
  typedef float  PixelType;
  typedef double ParametersValueType;

  typedef itk::Image< PixelType, 3 > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( fixedImageFileName );
  try
    {
    reader->UpdateLargestPossibleRegion();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }
  ImageType::Pointer fixedImage = reader->GetOutput();
  fixedImage->DisconnectPipeline();

  reader->SetFileName( movingImageFileName );
  try
    {
    reader->UpdateLargestPossibleRegion();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }
  ImageType::Pointer movingImage = reader->GetOutput();
  movingImage->DisconnectPipeline();


  typedef itk::FFTNormalizedCorrelationImageFilter< ImageType, ImageType >
    CorrelationFilterType;
  CorrelationFilterType::Pointer correlationFilter = CorrelationFilterType::New();
  correlationFilter->SetFixedImage( fixedImage );
  correlationFilter->SetMovingImage( movingImage );

  typedef itk::FFTPadImageFilter< ImageType > PadFilterType;
  PadFilterType::Pointer padFilter = PadFilterType::New();
  padFilter->SetInput( correlationFilter->GetOutput() );
  padFilter->SetSizeGreatestPrimeFactor( 2 );

  typedef itk::MinimumMaximumImageCalculator< ImageType > MaximumCalculatorType;
  MaximumCalculatorType::Pointer maximumCalculator = MaximumCalculatorType::New();
  maximumCalculator->SetImage( padFilter->GetOutput() );

  itk::HighPriorityRealTimeProbesCollector collector;
  const unsigned int numberOfIterations = 3;
  for( unsigned int ii = 0; ii < numberOfIterations; ++ii )
    {
    fixedImage->Modified();
    movingImage->Modified();
    collector.Start("NormalizedCorrelation");
    padFilter->UpdateLargestPossibleRegion();
    maximumCalculator->ComputeMaximum();
    collector.Stop("NormalizedCorrelation");
    }
  bool printSystemInfo = true;
  bool printReportHead = true;
  bool useTabs = false;
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );

  std::ofstream timingsFile( timingsFileName, std::ios::out );
  printSystemInfo = false;
  useTabs = true;
  collector.ExpandedReport( timingsFile, printSystemInfo, printReportHead, useTabs );

  std::cout << "Index of maximum: " << maximumCalculator->GetIndexOfMaximum() << std::endl;

  return EXIT_SUCCESS;
}
