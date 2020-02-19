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
#include "itkImageFileWriter.h"
#include "itkMinMaxCurvatureFlowImageFilter.h"

#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"
#include <fstream>

int
main(int argc, char * argv[])
{
  if (argc < 6)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile iterations threads inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
  }
  const std::string timingsFileName = ReplaceOccurrence(argv[1], "__DATESTAMP__", PerfDateStamp());
  const int         iterations = std::stoi(argv[2]);
  int               threads = std::stoi(argv[3]);
  const char *      inputImageFileName = argv[4];
  const char *      outputImageFileName = argv[5];

  if (threads > 0)
  {
    MultiThreaderName::SetGlobalDefaultNumberOfThreads(threads);
  }

  constexpr unsigned int Dimension = 3;
  using InputPixelType = unsigned char;
  using OutputPixelType = float;

  using InputImageType = itk::Image<InputPixelType, Dimension>;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;

  using ReaderType = itk::ImageFileReader<InputImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputImageFileName);
  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject & error)
  {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
  }
  InputImageType::Pointer inputImage = reader->GetOutput();
  inputImage->DisconnectPipeline();

  using FilterType = itk::MinMaxCurvatureFlowImageFilter<InputImageType, OutputImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetStencilRadius(1);
  filter->SetTimeStep(0.0625);
  filter->SetNumberOfIterations(3);
  filter->SetInput(inputImage);

  itk::HighPriorityRealTimeProbesCollector collector;
  for (int ii = 0; ii < iterations; ++ii)
  {
    inputImage->Modified();
    collector.Start("MinMaxCurvatureFlow");
    filter->UpdateLargestPossibleRegion();
    collector.Stop("MinMaxCurvatureFlow");
  }

  WriteExpandedReport(timingsFileName, collector, true, true, false);

  using WriterType = itk::ImageFileWriter<OutputImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(filter->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}
