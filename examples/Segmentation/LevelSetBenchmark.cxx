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
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

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

  const unsigned int Dimension = 3;
  typedef float  PixelType;

  typedef itk::Image< PixelType, 3 > ImageType;

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

  typedef itk::CurvatureAnisotropicDiffusionImageFilter< ImageType, ImageType >  SmoothingFilterType;
  SmoothingFilterType::Pointer smoothingFilter = SmoothingFilterType::New();
  smoothingFilter->SetInput( inputImage );
  smoothingFilter->SetNumberOfIterations( 5 );
  smoothingFilter->SetTimeStep( 0.0625 );
  smoothingFilter->SetConductanceParameter( 12.0 );

  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< ImageType, ImageType > GradientMagnitudeFilterType;
  GradientMagnitudeFilterType::Pointer gradientMagnitudeFilter = GradientMagnitudeFilterType::New();
  gradientMagnitudeFilter->SetInput( smoothingFilter->GetOutput() );
  gradientMagnitudeFilter->SetSigma( 1.0 );

  typedef itk::SigmoidImageFilter< ImageType, ImageType >  SigmoidFilterType;
  SigmoidFilterType::Pointer sigmoidFilter = SigmoidFilterType::New();
  sigmoidFilter->SetInput( gradientMagnitudeFilter->GetOutput() );
  sigmoidFilter->SetOutputMinimum( 0.0 );
  sigmoidFilter->SetOutputMaximum( 1.0 );
  sigmoidFilter->SetAlpha( -0.5 );
  sigmoidFilter->SetBeta( 3.0 );

  typedef itk::FastMarchingImageFilter< ImageType, ImageType > FastMarchingFilterType;
  FastMarchingFilterType::Pointer fastMarchingFilter = FastMarchingFilterType::New();
  typedef FastMarchingFilterType::NodeType NodeType;
  NodeType node;
  ImageType::IndexType seedPosition;
  seedPosition[0] = 77;
  seedPosition[1] = 112;
  seedPosition[2] = 35;
  node.SetIndex( seedPosition );
  node.SetValue( -5. );
  typedef FastMarchingFilterType::NodeContainer NodeContainerType;
  NodeContainerType::Pointer seeds = NodeContainerType::New();
  seeds->Initialize();
  seeds->InsertElement( 0, node );
  seedPosition[0] = 111;
  seedPosition[1] = 93;
  seedPosition[2] = 35;
  node.SetIndex( seedPosition );
  node.SetValue( -5. );
  seeds->InsertElement( 1, node );
  fastMarchingFilter->SetTrialPoints( seeds );
  fastMarchingFilter->SetSpeedConstant( 1.0 );
  fastMarchingFilter->SetOutputSize( inputImage->GetLargestPossibleRegion().GetSize() );
  fastMarchingFilter->SetOutputOrigin( inputImage->GetOrigin() );

  typedef itk::ShapeDetectionLevelSetImageFilter< ImageType, ImageType > ShapeDetectionFilterType;
  ShapeDetectionFilterType::Pointer shapeDetectionFilter = ShapeDetectionFilterType::New();
  shapeDetectionFilter->SetInput( fastMarchingFilter->GetOutput() );
  shapeDetectionFilter->SetPropagationScaling( 1.0 );
  shapeDetectionFilter->SetCurvatureScaling( 0.03 );
  shapeDetectionFilter->SetMaximumRMSError( 0.02 );
  shapeDetectionFilter->SetNumberOfIterations( 500 );
  shapeDetectionFilter->SetInput( fastMarchingFilter->GetOutput() );
  shapeDetectionFilter->SetFeatureImage( sigmoidFilter->GetOutput() );

  typedef unsigned char                           LabelPixelType;
  typedef itk::Image< LabelPixelType, Dimension > LabelImageType;
  typedef itk::BinaryThresholdImageFilter< ImageType, LabelImageType >
    ThresholdingFilterType;
  ThresholdingFilterType::Pointer thresholdingFilter = ThresholdingFilterType::New();
  thresholdingFilter->SetInput( shapeDetectionFilter->GetOutput() );
  thresholdingFilter->SetLowerThreshold( itk::NumericTraits< PixelType >::NonpositiveMin() );
  thresholdingFilter->SetUpperThreshold( 0.0 );
  thresholdingFilter->SetOutsideValue( 0 );
  thresholdingFilter->SetInsideValue( itk::NumericTraits< LabelPixelType >::max() );

  itk::HighPriorityRealTimeProbesCollector collector;
  for( int ii = 0; ii < iterations; ++ii )
    {
    inputImage->Modified();
    collector.Start("LevelSet");
    thresholdingFilter->UpdateLargestPossibleRegion();
    collector.Stop("LevelSet");
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
  writer->SetInput( thresholdingFilter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
