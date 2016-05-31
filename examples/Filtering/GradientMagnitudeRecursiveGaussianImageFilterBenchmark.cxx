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
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"

#include "itkHighPriorityRealTimeProbesCollector.h"

#include <fstream>

int main( int argc, char * argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile outputImageFile timingsFile" << std::endl;
    return EXIT_FAILURE;
    }
  const char * inputImageFileName = argv[1];
  const char * outputImageFileName = argv[2];
  const char * timingsFileName = argv[3];

  const unsigned int Dimension = 3;
  typedef unsigned char PixelType;

  typedef itk::Image< PixelType, 3 > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImageFileName );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }
  ImageType::Pointer inputImage = reader->GetOutput();

  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< ImageType, ImageType >  FilterType;
  FilterType::Pointer filter = FilterType::New();
  ImageType::SizeType radius;
  radius.Fill( 2 );
  filter->SetSigma( 3.0 );
  filter->SetInput( inputImage );
  // Cache disk IO
  filter->UpdateLargestPossibleRegion();

  itk::HighPriorityRealTimeProbesCollector collector;
  const unsigned int numberOfIterations = 3;
  for( unsigned int ii = 0; ii < numberOfIterations; ++ii )
    {
    inputImage->Modified();
    collector.Start("GradientMagnitude");
    filter->UpdateLargestPossibleRegion();
    collector.Stop("GradientMagnitude");
    }
  bool printSystemInfo = true;
  bool printReportHead = true;
  bool useTabs = false;
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );

  std::ofstream timingsFile( timingsFileName, std::ios::out );
  printSystemInfo = false;
  useTabs = true;
  collector.ExpandedReport( timingsFile, printSystemInfo, printReportHead, useTabs );

  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
