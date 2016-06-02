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
#include "itkTransformFileWriter.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkCommand.h"

#include "itkHighPriorityRealTimeProbesCollector.h"

#include <fstream>

class CommandIterationUpdate : public itk::Command
{
public:
  typedef CommandIterationUpdate   Self;
  typedef itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  CommandIterationUpdate() {};

public:

  typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;
  typedef const OptimizerType*                               OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE
  {
    OptimizerPointer optimizer = static_cast< OptimizerPointer >( object );

    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }

    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
  }
};


int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " fixedImageFile movingImageFile outputTransformFileName timingsFile" << std::endl;
    return EXIT_FAILURE;
    }
  const char * fixedImageFileName = argv[1];
  const char * movingImageFileName = argv[2];
  const char * outputTransformFileName = argv[3];
  const char * timingsFileName = argv[4];

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


  typedef itk::RegularStepGradientDescentOptimizerv4< ParametersValueType > OptimizerType;
  OptimizerType::Pointer optimizer = OptimizerType::New();
  optimizer->SetLearningRate( 4.0 );
  optimizer->SetMinimumStepLength( 0.001 );
  optimizer->SetRelaxationFactor( 0.5 );
  optimizer->SetNumberOfIterations( 200 );
  //CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  //optimizer->AddObserver( itk::IterationEvent(), observer );

  typedef itk::MeanSquaresImageToImageMetricv4< ImageType, ImageType > MetricType;
  MetricType::Pointer metric = MetricType::New();

  typedef itk::TranslationTransform< ParametersValueType, Dimension > TransformType;

  typedef itk::ImageRegistrationMethodv4< ImageType, ImageType, TransformType > RegistrationType;
  RegistrationType::Pointer registration = RegistrationType::New();
  registration->SetMetric( metric );
  registration->SetOptimizer( optimizer );

  TransformType::Pointer movingInitialTransform = TransformType::New();
  TransformType::ParametersType initialParameters(
    movingInitialTransform->GetNumberOfParameters() );
  initialParameters.Fill( 0.0 );
  movingInitialTransform->SetParameters( initialParameters );
  registration->SetMovingInitialTransform( movingInitialTransform );

  TransformType::Pointer   identityTransform = TransformType::New();
  identityTransform->SetIdentity();
  registration->SetFixedInitialTransform( identityTransform );

  TransformType::Pointer optimizedTransform = TransformType::New();
  optimizedTransform->SetParameters( initialParameters );
  registration->SetInitialTransform( optimizedTransform );

  const unsigned int numberOfLevels = 1;

  RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
  shrinkFactorsPerLevel.SetSize( 1 );
  shrinkFactorsPerLevel[0] = 1;

  RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
  smoothingSigmasPerLevel.SetSize( 1 );
  smoothingSigmasPerLevel[0] = 0;

  registration->SetNumberOfLevels ( numberOfLevels );
  registration->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
  registration->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );

  RegistrationType::MetricSamplingStrategyType samplingStrategy = RegistrationType::RANDOM;
  registration->SetMetricSamplingStrategy( samplingStrategy );
  registration->SetMetricSamplingPercentage( 0.03 );

  registration->SetFixedImage( fixedImage );
  registration->SetMovingImage( movingImage );

  typedef itk::RegistrationParameterScalesFromPhysicalShift<MetricType> ScalesEstimatorType;
  ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
  scalesEstimator->SetMetric( metric );
  scalesEstimator->SetTransformForward( true );
  optimizer->SetScalesEstimator( scalesEstimator );
  optimizer->SetDoEstimateLearningRateOnce( true );

  itk::HighPriorityRealTimeProbesCollector collector;
  const unsigned int numberOfIterations = 3;
  for( unsigned int ii = 0; ii < numberOfIterations; ++ii )
    {
    collector.Start("RegistrationFramework");
    optimizedTransform->SetParameters( initialParameters );
    registration->SetInitialTransform( optimizedTransform );
    registration->Update();
    collector.Stop("RegistrationFramework");
    }
  bool printSystemInfo = true;
  bool printReportHead = true;
  bool useTabs = false;
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );

  std::ofstream timingsFile( timingsFileName, std::ios::out );
  printSystemInfo = false;
  useTabs = true;
  collector.ExpandedReport( timingsFile, printSystemInfo, printReportHead, useTabs );

  TransformType::ConstPointer transform = registration->GetTransform();

  typedef itk::TransformFileWriterTemplate< ParametersValueType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputTransformFileName );
  writer->SetInput( transform );
  writer->Update();

  return EXIT_SUCCESS;
}
