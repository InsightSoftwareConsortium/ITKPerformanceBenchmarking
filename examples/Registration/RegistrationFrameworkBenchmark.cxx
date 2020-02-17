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
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"

#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"


class CommandIterationUpdate : public itk::Command
{
public:
  using Self = CommandIterationUpdate;
  using Superclass = itk::Command;
  using Pointer = itk::SmartPointer<Self>;
  itkNewMacro(Self);

protected:
  CommandIterationUpdate() = default;

public:
  using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
  using OptimizerPointer = const OptimizerType *;

  void
  Execute(itk::Object * caller, const itk::EventObject & event) override
  {
    Execute((const itk::Object *)caller, event);
  }

  void
  Execute(const itk::Object * object, const itk::EventObject & event) override
  {
    auto optimizer = static_cast<OptimizerPointer>(object);

    if (!itk::IterationEvent().CheckEvent(&event))
    {
      return;
    }

    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
  }
};


int
main(int argc, char * argv[])
{
  if (argc < 7)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile iterations threads fixedImageFile movingImageFile outputTransformFileName"
              << std::endl;
    return EXIT_FAILURE;
  }
  const std::string timingsFileName = ReplaceOccurrence(argv[1], "__DATESTAMP__", PerfDateStamp());
  const int         iterations = std::stoi(argv[2]);
  int               threads = std::stoi(argv[3]);
  const char *      fixedImageFileName = argv[4];
  const char *      movingImageFileName = argv[5];
  const char *      outputTransformFileName = argv[6];

  if (threads > 0)
  {
    MultiThreaderName::SetGlobalDefaultNumberOfThreads(threads);
  }

  constexpr unsigned int Dimension = 3;
  using PixelType = float;
  using ParametersValueType = double;

  using ImageType = itk::Image<PixelType, 3>;

  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fixedImageFileName);
  try
  {
    reader->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject & error)
  {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
  }
  ImageType::Pointer fixedImage = reader->GetOutput();
  fixedImage->DisconnectPipeline();

  reader->SetFileName(movingImageFileName);
  try
  {
    reader->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject & error)
  {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
  }
  ImageType::Pointer movingImage = reader->GetOutput();
  movingImage->DisconnectPipeline();


  using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<ParametersValueType>;
  OptimizerType::Pointer optimizer = OptimizerType::New();
  optimizer->SetLearningRate(4.0);
  optimizer->SetMinimumStepLength(0.001);
  optimizer->SetRelaxationFactor(0.5);
  optimizer->SetNumberOfIterations(200);
  // CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  // optimizer->AddObserver( itk::IterationEvent(), observer );

  using MetricType = itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType>;
  MetricType::Pointer metric = MetricType::New();

  using TransformType = itk::TranslationTransform<ParametersValueType, Dimension>;

  using RegistrationType = itk::ImageRegistrationMethodv4<ImageType, ImageType, TransformType>;
  RegistrationType::Pointer registration = RegistrationType::New();
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);

  TransformType::Pointer        movingInitialTransform = TransformType::New();
  TransformType::ParametersType initialParameters(movingInitialTransform->GetNumberOfParameters());
  initialParameters.Fill(0.0);
  movingInitialTransform->SetParameters(initialParameters);
  registration->SetMovingInitialTransform(movingInitialTransform);

  TransformType::Pointer identityTransform = TransformType::New();
  identityTransform->SetIdentity();
  registration->SetFixedInitialTransform(identityTransform);

  TransformType::Pointer optimizedTransform = TransformType::New();
  optimizedTransform->SetParameters(initialParameters);
  registration->SetInitialTransform(optimizedTransform);

  constexpr unsigned int numberOfLevels = 1;

  RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
  shrinkFactorsPerLevel.SetSize(1);
  shrinkFactorsPerLevel[0] = 1;

  RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
  smoothingSigmasPerLevel.SetSize(1);
  smoothingSigmasPerLevel[0] = 0;

  registration->SetNumberOfLevels(numberOfLevels);
  registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
  registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

  RegistrationType::MetricSamplingStrategyType samplingStrategy = RegistrationType::RANDOM;
  registration->SetMetricSamplingStrategy(samplingStrategy);
  registration->SetMetricSamplingPercentage(0.03);

  registration->SetFixedImage(fixedImage);
  registration->SetMovingImage(movingImage);

  using ScalesEstimatorType = itk::RegistrationParameterScalesFromPhysicalShift<MetricType>;
  ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
  scalesEstimator->SetMetric(metric);
  scalesEstimator->SetTransformForward(true);
  optimizer->SetScalesEstimator(scalesEstimator);
  optimizer->SetDoEstimateLearningRateOnce(true);

  itk::HighPriorityRealTimeProbesCollector collector;
  for (int ii = 0; ii < iterations; ++ii)
  {
    collector.Start("RegistrationFramework");
    optimizedTransform->SetParameters(initialParameters);
    registration->SetInitialTransform(optimizedTransform);
    registration->Update();
    collector.Stop("RegistrationFramework");
  }

  WriteExpandedReport(timingsFileName, collector, true, true, false);
  TransformType::ConstPointer transform = registration->GetTransform();

  using WriterType = itk::TransformFileWriterTemplate<ParametersValueType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputTransformFileName);
  writer->SetInput(transform);
  writer->Update();

  return EXIT_SUCCESS;
}
