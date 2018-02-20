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
#include "itkTransformFileWriter.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkCommand.h"

#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"

#include <fstream>

class CommandIterationUpdate : public itk::Command
{
public:
  using Self = CommandIterationUpdate;
  using Superclass = itk::Command;
  using Pointer = itk::SmartPointer<CommandIterationUpdate>;
  itkNewMacro( CommandIterationUpdate );
protected:
  CommandIterationUpdate() {};

  using ImageType = itk::Image< float, 2 >;
  using VectorPixelType = itk::Vector< float, 2 >;
  using DisplacementFieldType = itk::Image<  VectorPixelType, 2 >;

  using RegistrationFilterType = itk::DemonsRegistrationFilter<
                              ImageType,
                              ImageType,
                              DisplacementFieldType>;

public:
  virtual void Execute(itk::Object *caller, const itk::EventObject & event) override
  {
    Execute( (const itk::Object *)caller, event);
  }

  virtual void Execute(const itk::Object * object, const itk::EventObject & event) override
  {
    const RegistrationFilterType * filter = static_cast< const RegistrationFilterType * >( object );
    if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
      return;
      }
    std::cout << filter->GetMetric() << std::endl;
  }
};


int main( int argc, char * argv[] )
{
  if( argc < 7 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile threads fixedImageFile movingImageFile outputDisplacementFieldFileName" << std::endl;
    return EXIT_FAILURE;
    }
  const std::string timingsFileName = ReplaceOccurrence( argv[1], "__DATESTAMP__", PerfDateStamp());
  const int iterations = atoi( argv[2] );
  int threads = atoi( argv[3] );
  const char * fixedImageFileName = argv[4];
  const char * movingImageFileName = argv[5];
  const char * outputFileName = argv[6];

  if( threads > 0 )
    {
    itk::MultiThreader::SetGlobalDefaultNumberOfThreads( threads );
    }

  constexpr unsigned int Dimension = 3;
  using PixelType = float;
  using ParametersValueType = double;

  using ImageType = itk::Image< PixelType, 3 >;

  using ReaderType = itk::ImageFileReader< ImageType >;
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


  using VectorPixelType = itk::Vector< float, Dimension >;
  using DisplacementFieldType = itk::Image<  VectorPixelType, Dimension >;
  using RegistrationFilterType = itk::DemonsRegistrationFilter<
                                ImageType,
                                ImageType,
                                DisplacementFieldType >;
  RegistrationFilterType::Pointer filter = RegistrationFilterType::New();

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  filter->AddObserver( itk::IterationEvent(), observer );
  filter->SetFixedImage( fixedImage );
  filter->SetMovingImage( movingImage );
  // More interations are required for convergence, but limit the iterations
  // to keep runtime reasonable
  filter->SetNumberOfIterations( 5 );
  filter->SetStandardDeviations( 1.0 );
  filter->SmoothUpdateFieldOff();
  filter->SmoothDisplacementFieldOn();

  itk::HighPriorityRealTimeProbesCollector collector;
  for( int ii = 0; ii < iterations; ++ii )
    {
    fixedImage->Modified();
    movingImage->Modified();
    collector.Start("DemonsRegistration");
    filter->UpdateLargestPossibleRegion();
    collector.Stop("DemonsRegistration");
    }

  WriteExpandedReport(timingsFileName,collector,true,true,false);

  using WriterType = itk::ImageFileWriter< DisplacementFieldType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
