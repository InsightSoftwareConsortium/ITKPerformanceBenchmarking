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
#include "itkCurvatureFlowImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkRelabelComponentImageFilter.h"

#include "itkHighPriorityRealTimeProbesCollector.h"

#include <fstream>


int main( int argc, char * argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile iterations threads inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }
  const char * timingsFileName = argv[1];
  const int iterations = atoi( argv[2] );
  int threads = atoi( argv[3] );
  const char * inputImageFileName = argv[4];
  const char * outputImageFileName = argv[5];

  if( threads > 0 )
    {
    itk::MultiThreader::SetGlobalDefaultNumberOfThreads( threads );
    }

  constexpr unsigned int Dimension = 3;
  using PixelType = float;

  using ImageType = itk::Image< PixelType, 3 >;

  using ReaderType = itk::ImageFileReader< ImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImageFileName );
  try
    {
    reader->UpdateLargestPossibleRegion();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }
  ImageType::Pointer inputImage = reader->GetOutput();
  inputImage->DisconnectPipeline();

  using SmoothingFilterType = itk::CurvatureFlowImageFilter< ImageType, ImageType >;
  SmoothingFilterType::Pointer smoothingFilter = SmoothingFilterType::New();
  smoothingFilter->SetInput( inputImage );
  smoothingFilter->SetNumberOfIterations( 2 );
  smoothingFilter->SetTimeStep( 0.05 );

  using GradientMagnitudeFilterType = itk::GradientMagnitudeRecursiveGaussianImageFilter< ImageType, ImageType >;
  GradientMagnitudeFilterType::Pointer gradientMagnitudeFilter = GradientMagnitudeFilterType::New();
  gradientMagnitudeFilter->SetInput( smoothingFilter->GetOutput() );
  gradientMagnitudeFilter->SetSigma( 5.0 );

  using WatershedFilterType = itk::WatershedImageFilter< ImageType >;
  WatershedFilterType::Pointer watershedFilter = WatershedFilterType::New();
  watershedFilter->SetInput( gradientMagnitudeFilter->GetOutput() );
  watershedFilter->SetThreshold( 0.0001 );
  watershedFilter->SetLevel( 0.3 );

  using LabelImageType = WatershedFilterType::OutputImageType;
  using RelabelFilterType = itk::RelabelComponentImageFilter< LabelImageType, LabelImageType >;
  RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
  relabelFilter->SetInput( watershedFilter->GetOutput() );
  relabelFilter->SetMinimumObjectSize( 200 );

  itk::HighPriorityRealTimeProbesCollector collector;
  for( int ii = 0; ii < iterations; ++ii )
    {
    inputImage->Modified();
    collector.Start("Watershed");
    relabelFilter->UpdateLargestPossibleRegion();
    collector.Stop("Watershed");
    }
  bool printSystemInfo = true;
  bool printReportHead = true;
  bool useTabs = false;
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );

  std::ofstream timingsFile( timingsFileName, std::ios::out );
  printSystemInfo = false;
  useTabs = true;
  collector.ExpandedReport( timingsFile, printSystemInfo, printReportHead, useTabs );

  using WriterType = itk::ImageFileWriter< LabelImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( relabelFilter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
