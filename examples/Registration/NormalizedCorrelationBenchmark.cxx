/*=========================================================================
 *
 *  Copyright NumFOCUS
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

#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"


int
main(int argc, char * argv[])
{
  if (argc < 6)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile iterations threads fixedImageFile movingImageFile" << std::endl;
    return EXIT_FAILURE;
  }
  const std::string timingsFileName = ReplaceOccurrence(argv[1], "__DATESTAMP__", PerfDateStamp());
  const int         iterations = std::stoi(argv[2]);
  int               threads = std::stoi(argv[3]);
  const std::string fixedImageFileName = argv[4];
  const std::string movingImageFileName = argv[5];

  if (threads > 0)
  {
    MultiThreaderName::SetGlobalDefaultNumberOfThreads(threads);
  }

  constexpr unsigned int Dimension = 3;
  using PixelType = float;

  using ImageType = itk::Image<PixelType, Dimension>;

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


  using CorrelationFilterType = itk::FFTNormalizedCorrelationImageFilter<ImageType, ImageType>;
  CorrelationFilterType::Pointer correlationFilter = CorrelationFilterType::New();
  correlationFilter->SetFixedImage(fixedImage);
  correlationFilter->SetMovingImage(movingImage);

  using PadFilterType = itk::FFTPadImageFilter<ImageType>;
  PadFilterType::Pointer padFilter = PadFilterType::New();
  padFilter->SetInput(correlationFilter->GetOutput());
  padFilter->SetSizeGreatestPrimeFactor(2);

  using MaximumCalculatorType = itk::MinimumMaximumImageCalculator<ImageType>;
  MaximumCalculatorType::Pointer maximumCalculator = MaximumCalculatorType::New();
  maximumCalculator->SetImage(padFilter->GetOutput());

  itk::HighPriorityRealTimeProbesCollector collector;
  for (int ii = 0; ii < iterations; ++ii)
  {
    fixedImage->Modified();
    movingImage->Modified();
    collector.Start("NormalizedCorrelation");
    padFilter->UpdateLargestPossibleRegion();
    maximumCalculator->ComputeMaximum();
    collector.Stop("NormalizedCorrelation");
  }

  WriteExpandedReport(timingsFileName, collector, true, true, false);

  std::cout << "Index of maximum: " << maximumCalculator->GetIndexOfMaximum() << std::endl;

  return EXIT_SUCCESS;
}
