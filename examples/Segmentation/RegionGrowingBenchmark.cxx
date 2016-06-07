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
#include "itkConfidenceConnectedImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkBinaryFillholeImageFilter.h"

#include "itkHighPriorityRealTimeProbesCollector.h"

#include <fstream>


int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile threads inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }
  const char * timingsFileName = argv[1];
  int threads = atoi( argv[2] );
  const char * inputImageFileName = argv[3];
  const char * outputImageFileName = argv[4];

  if( threads > 0 )
    {
    itk::MultiThreader::SetGlobalDefaultNumberOfThreads( threads );
    }

  const unsigned int Dimension = 3;
  typedef float                                   PixelType;
  typedef itk::Image< PixelType, Dimension >      ImageType;
  typedef unsigned char                           LabelPixelType;
  typedef itk::Image< LabelPixelType, Dimension > LabelImageType;


  typedef itk::ImageFileReader< ImageType > ReaderType;
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

  typedef itk::CurvatureFlowImageFilter< ImageType, ImageType > SmoothingFilterType;
  SmoothingFilterType::Pointer smoothingFilter = SmoothingFilterType::New();
  smoothingFilter->SetInput( inputImage );
  smoothingFilter->SetNumberOfIterations( 2 );
  smoothingFilter->SetTimeStep( 0.05 );

  typedef itk::ConfidenceConnectedImageFilter< ImageType, LabelImageType > ConfidenceConnectedFilterType;
  ConfidenceConnectedFilterType::Pointer confidenceConnectedFilter = ConfidenceConnectedFilterType::New();
  confidenceConnectedFilter->SetInput( smoothingFilter->GetOutput() );
  confidenceConnectedFilter->SetMultiplier( 2.2 );
  confidenceConnectedFilter->SetNumberOfIterations( 10 );
  confidenceConnectedFilter->SetInitialNeighborhoodRadius( 2 );
  confidenceConnectedFilter->SetReplaceValue( itk::NumericTraits< LabelPixelType >::max() );

  ImageType::IndexType index1;
  index1[0] = 118;
  index1[1] = 133;
  index1[2] = 92;
  confidenceConnectedFilter->AddSeed( index1 );

  ImageType::IndexType index2;
  index2[0] = 63;
  index2[1] = 135;
  index2[2] = 94;
  confidenceConnectedFilter->AddSeed( index2 );

  ImageType::IndexType index3;
  index3[0] = 63;
  index3[1] = 157;
  index3[2] = 90;
  confidenceConnectedFilter->AddSeed( index3 );

  ImageType::IndexType index4;
  index4[0] = 111;
  index4[1] = 150;
  index4[2] = 90;
  confidenceConnectedFilter->AddSeed( index4 );

  ImageType::IndexType index5;
  index5[0] = 111;
  index5[1] = 50;
  index5[2] = 88;
  confidenceConnectedFilter->AddSeed( index5 );

  typedef itk::BinaryFillholeImageFilter< LabelImageType > FillholeFilterType;
  FillholeFilterType::Pointer fillholeFilter = FillholeFilterType::New();
  fillholeFilter->SetInput( confidenceConnectedFilter->GetOutput() );
  fillholeFilter->SetForegroundValue( confidenceConnectedFilter->GetReplaceValue() );

  itk::HighPriorityRealTimeProbesCollector collector;
  const unsigned int numberOfIterations = 3;
  for( unsigned int ii = 0; ii < numberOfIterations; ++ii )
    {
    inputImage->Modified();
    collector.Start("RegionGrowing");
    fillholeFilter->UpdateLargestPossibleRegion();
    collector.Stop("RegionGrowing");
    }
  bool printSystemInfo = true;
  bool printReportHead = true;
  bool useTabs = false;
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );

  std::ofstream timingsFile( timingsFileName, std::ios::out );
  printSystemInfo = false;
  useTabs = true;
  collector.ExpandedReport( timingsFile, printSystemInfo, printReportHead, useTabs );

  typedef itk::ImageFileWriter< LabelImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( fillholeFilter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
