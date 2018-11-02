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
#include "itkMorphologicalWatershedImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkRelabelComponentImageFilter.h"

#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"

#include <fstream>


int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile iterations inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }
  const std::string timingsFileName = ReplaceOccurrence( argv[1], "__DATESTAMP__", PerfDateStamp());
  const int iterations = std::stoi( argv[2] );
  const char * inputImageFileName = argv[3];
  const char * outputImageFileName = argv[4];

  constexpr unsigned int Dimension = 3;
  using PixelType = float;

  using ImageType = itk::Image< PixelType, Dimension >;
  using LabelImageType = itk::Image< unsigned long long, Dimension >;

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

  using GradientMagnitudeFilterType = itk::GradientMagnitudeRecursiveGaussianImageFilter< ImageType, ImageType >;
  GradientMagnitudeFilterType::Pointer gradientMagnitudeFilter = GradientMagnitudeFilterType::New();
  gradientMagnitudeFilter->SetInput( inputImage );
  gradientMagnitudeFilter->SetSigma( 5.0 );

  using WatershedFilterType = itk::MorphologicalWatershedImageFilter< ImageType, LabelImageType >;
  WatershedFilterType::Pointer watershedFilter = WatershedFilterType::New();
  watershedFilter->SetInput( gradientMagnitudeFilter->GetOutput() );
  watershedFilter->SetLevel( 0.3 );
  watershedFilter->FullyConnectedOn();
  watershedFilter->MarkWatershedLineOff();

  itk::HighPriorityRealTimeProbesCollector collector;
  for( int ii = 0; ii < iterations; ++ii )
    {
    inputImage->Modified();
    collector.Start("Watershed");
    watershedFilter->UpdateLargestPossibleRegion();
    collector.Stop("Watershed");
    }

  WriteExpandedReport(timingsFileName, collector, true, true, false);

  using WriterType = itk::ImageFileWriter< LabelImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImageFileName );
  writer->SetInput( watershedFilter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
