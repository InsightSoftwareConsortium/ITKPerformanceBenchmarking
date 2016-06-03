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

#include <fstream>

class CommandIterationUpdate : public itk::Command
{
public:
  typedef  CommandIterationUpdate                     Self;
  typedef  itk::Command                               Superclass;
  typedef  itk::SmartPointer<CommandIterationUpdate>  Pointer;
  itkNewMacro( CommandIterationUpdate );
protected:
  CommandIterationUpdate() {};

  typedef itk::Image< float, 2 >            ImageType;
  typedef itk::Vector< float, 2 >           VectorPixelType;
  typedef itk::Image<  VectorPixelType, 2 > DisplacementFieldType;

  typedef itk::DemonsRegistrationFilter<
                              ImageType,
                              ImageType,
                              DisplacementFieldType>   RegistrationFilterType;

public:
  virtual void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE
  {
    Execute( (const itk::Object *)caller, event);
  }

  virtual void Execute(const itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE
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
  if( argc < 6 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile threads fixedImageFile movingImageFile outputDisplacementFieldFileName" << std::endl;
    return EXIT_FAILURE;
    }
  const char * timingsFileName = argv[1];
  int threads = atoi( argv[2] );
  const char * fixedImageFileName = argv[3];
  const char * movingImageFileName = argv[4];
  const char * outputFileName = argv[5];

  if( threads > 0 )
    {
    itk::MultiThreader::SetGlobalDefaultNumberOfThreads( threads );
    }

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


  typedef itk::Vector< float, Dimension >                VectorPixelType;
  typedef itk::Image<  VectorPixelType, Dimension >      DisplacementFieldType;
  typedef itk::DemonsRegistrationFilter<
                                ImageType,
                                ImageType,
                                DisplacementFieldType > RegistrationFilterType;
  RegistrationFilterType::Pointer filter = RegistrationFilterType::New();
  // Software Guide : EndCodeSnippet

  // Create the Command observer and register it with the registration filter.
  //
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
  const unsigned int numberOfIterations = 3;
  for( unsigned int ii = 0; ii < numberOfIterations; ++ii )
    {
    fixedImage->Modified();
    movingImage->Modified();
    collector.Start("DemonsRegistration");
    filter->UpdateLargestPossibleRegion();
    collector.Stop("DemonsRegistration");
    }
  bool printSystemInfo = true;
  bool printReportHead = true;
  bool useTabs = false;
  collector.Report( std::cout, printSystemInfo, printReportHead, useTabs );

  std::ofstream timingsFile( timingsFileName, std::ios::out );
  printSystemInfo = false;
  useTabs = true;
  collector.ExpandedReport( timingsFile, printSystemInfo, printReportHead, useTabs );

  typedef itk::ImageFileWriter< DisplacementFieldType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
