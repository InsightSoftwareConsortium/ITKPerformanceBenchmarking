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
//
// \author Denis P. Shamonin, Marius Staring and Niels Dekker.
// Division of Image Processing, Department of Radiology, Leiden, The Netherlands
//
// \note This work was funded by the Netherlands Organisation for
// Scientific Research (NWO NRG-2010.02 and NWO 639.021.124).
//

// ITK command line parsing
#include "itksys/CommandLineArguments.hxx"

// ITK helper includes
#include "itkContinuousIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMinimumMaximumImageCalculator.h"

// ITK Resample include file for benchmarking
#include "itkResampleImageFilter.h"

// ITK interpolate includes
#include "itkBSplineInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

// ITK extrapolate includes
#include "itkNearestNeighborExtrapolateImageFunction.h"

// ITK Transforms includes
#include "itkAffineTransform.h"
#include "itkBSplineTransform.h"
#include "itkCompositeTransform.h"
#include "itkEuler2DTransform.h"
#include "itkEuler3DTransform.h"
#include "itkIdentityTransform.h"
#include "itkSimilarity2DTransform.h"
#include "itkSimilarity3DTransform.h"
#include "itkTranslationTransform.h"

// Includes for ITK performance benchmarking
#include "PerformanceBenchmarkingUtilities.h"
#include "itkHighPriorityRealTimeProbesCollector.h"

// Standard Library includes
#include <algorithm>
#include <random>
#include <vector>

namespace
{
// Create the input image when input image is not provided
template< typename ImageType >
typename ImageType::Pointer
CreateInputImage( const std::vector< int >& imageSizes )
{
  auto image = ImageType::New();
  typename ImageType::SizeType imageSize;
  for ( std::size_t i = 0; i < imageSizes.size(); ++i )
    {
      imageSize[i] = imageSizes[i];
    }
  image->SetRegions( typename ImageType::RegionType( imageSize ) );
  image->Allocate();

  ImageType::SpacingType spacing;
  ImageType::PointType   origin;

  if ( ImageType::ImageDimension < 3 )
    {
      spacing.Fill( 1.0 );
      origin.Fill( 0.0 );
    }
  else
    {
      // Use the same image properties as in original Insight Journal article
      spacing[0] = 0.660156;
      spacing[1] = 0.660156;
      spacing[2] = 1.5;

      origin[0] = -157.67;
      origin[1] = -362.67;
      origin[2] = -1198.6;
    }

  image->SetSpacing( spacing );
  image->SetOrigin( origin );

  // Fill the image with some test gradient pattern
  auto value = itk::NumericTraits< ImageType::PixelType >::Zero;
  itk::ImageRegionIterator< ImageType > iter( image, image->GetLargestPossibleRegion() );
  while ( !iter.IsAtEnd() )
    {
      switch ( ImageType::ImageDimension )
        {
          case 1:
            iter.Set( ++value );
            break;
          case 2:
            iter.Set( iter.GetIndex()[0] - iter.GetIndex()[1] );
            break;
          case 3:
            iter.Set( iter.GetIndex()[0] - iter.GetIndex()[1] + iter.GetIndex()[2] );
            break;
          default:
            break;
        }

      ++iter;
    }

  return image;
}

// Helper functions used by ValidateArguments()
bool
IsStringInVector( const std::string& str, const std::vector< std::string >& vec )
{
  return std::any_of(vec.begin(), vec.end(), [&str]( const std::string& i ) { return i == str; } );
}

bool
AreAllStringsInVector( const std::vector< std::string >& args, const std::vector< std::string >& in )
{
  bool result = true;
  for ( const auto& arg : args )
    {
      result &= IsStringInVector( arg, in );
    }
  return result;
}

void
ReplaceUnderscoreWithSpace( std::string& arg )
{
  // Get rid of the possible "_" in arg.
  const auto pos = arg.find( '_' );

  if ( pos != std::string::npos)
    {
      arg[pos] = ' ';
    }
}

bool
GetImageProperties( const std::string& filename, std::string& pixeltype, std::string& componenttype,
                    unsigned int& dimension, unsigned int& numberofcomponents, std::vector< unsigned int >& imagesize )
{
  // Dummy image type.
  const unsigned int DummyDimension = 3;

  using DummyPixelType = short;
  using DummyImageType = itk::Image< DummyPixelType, DummyDimension >;

  // The reader
  using ReaderType = itk::ImageFileReader< DummyImageType >;

  // Image header information class
  using ImageIOBaseType = itk::ImageIOBase;

  // Create a reader
  auto reader = ReaderType::New();
  reader->SetFileName( filename.c_str() );

  // Generate all information
  try
    {
      reader->UpdateOutputInformation();
    }
  catch ( const itk::ExceptionObject& exceptionObject )
    {
      std::cerr << "Caught ITK exception: " << exceptionObject << std::endl;
      return false;
    }

  // Extract the ImageIO from the reader
  ImageIOBaseType::Pointer imageIOBase = reader->GetImageIO();

  // Get the component type, number of components, dimension and pixel type
  dimension = imageIOBase->GetNumberOfDimensions();
  numberofcomponents = imageIOBase->GetNumberOfComponents();
  componenttype = imageIOBase->GetComponentTypeAsString( imageIOBase->GetComponentType() );
  ReplaceUnderscoreWithSpace( componenttype );
  pixeltype = imageIOBase->GetPixelTypeAsString( imageIOBase->GetPixelType() );

  // Get the image size
  imagesize.resize( dimension );
  for ( unsigned int i = 0; i < dimension; ++i )
    {
      imagesize[i] = imageIOBase->GetDimensions( i );
    }

  // Check inputPixelType
  const std::vector< std::string > validTypes = { "unsigned char", "char", "unsigned short", "short",
                                                  "unsigned long", "long", "float", "long" };
  if ( !IsStringInVector( componenttype, validTypes ) )
    {
      // In this case an illegal pixeltype is found
      std::cerr << "ERROR: while determining image properties!"
        "The found componenttype is \"" << componenttype << "\", which is not supported." << std::endl;
      return false;
    }

  return true;
}

// Command arguments storage
class Parameters
{
public:
  // Constructor
  Parameters()
    : timingsFileName( "" )
    , iterations( 1 )
    , threads( 0 )
    , inputFileName( "" )
    , interpolator( "Linear" )
    , extrapolator( "" )
    , useCompositeTransform( false )
    , splineOrderInterpolator( 3 )
    , transformsPrecision( "float" )
  {}

  // Benchmark main settings
  std::string timingsFileName;
  int         iterations;
  int         threads;

  // Input, output images
  std::string                inputFileName;
  std::vector< int >         imageSizes;
  std::vector< std::string > outputFileNames;

  // Interpolator, extrapolator and transforms settings
  std::string                interpolator;
  std::string                extrapolator;
  std::vector< std::string > transforms;
  bool                       useCompositeTransform;
  int                        splineOrderInterpolator;
  std::string                transformsPrecision;
};

template< typename TransformType, typename InputImageType >
std::string
GetTransformName( typename TransformType::Pointer& transform )
{
  std::ostringstream ost;
  ost << transform->GetNameOfClass();

  // Try float transform
  using FloatCompositeTransformType = itk::CompositeTransform< float, InputImageType::ImageDimension >;
  const auto* floatCompositeTransform = dynamic_cast< const FloatCompositeTransformType* >( transform.GetPointer() );

  if ( floatCompositeTransform )
    {
      ost << " [";
      for ( std::size_t i = 0; i < floatCompositeTransform->GetNumberOfTransforms(); ++i )
        {
          ost << floatCompositeTransform->GetNthTransform( i )->GetNameOfClass();
          if ( i != floatCompositeTransform->GetNumberOfTransforms() - 1 )
            {
              ost << ", ";
            }
        }
      ost << "]";
    }
  else
    {
      // Try double transform
      using DoubleCompositeTransformType = itk::CompositeTransform< double, InputImageType::ImageDimension >;
      const auto* doubleCompositeTransform =
        dynamic_cast< const DoubleCompositeTransformType* >( transform.GetPointer() );

      if ( doubleCompositeTransform )
        {
          ost << " [";
          for ( std::size_t i = 0; i < doubleCompositeTransform->GetNumberOfTransforms(); ++i )
            {
              ost << doubleCompositeTransform->GetNthTransform( i )->GetNameOfClass();
              if ( i != doubleCompositeTransform->GetNumberOfTransforms() - 1 )
                {
                  ost << ", ";
                }
            }
          ost << "]";
        }
    }

  return ost.str();
}

template< typename InputImageType >
typename InputImageType::PointType
ComputeCenterOfTheImage( const typename InputImageType::ConstPointer& image )
{
  const unsigned int Dimension = image->GetImageDimension();

  const typename InputImageType::SizeType  size = image->GetLargestPossibleRegion().GetSize();
  const typename InputImageType::IndexType index = image->GetLargestPossibleRegion().GetIndex();

  using ContinuousIndexType = itk::ContinuousIndex< double, InputImageType::ImageDimension >;
  ContinuousIndexType centerAsContInd;
  for ( std::size_t i = 0; i < Dimension; ++i )
    {
      centerAsContInd[i] = static_cast< double >( index[i] ) + static_cast< double >( size[i] - 1 ) / 2.0;
    }

  typename InputImageType::PointType center;
  image->TransformContinuousIndexToPhysicalPoint( centerAsContInd, center );
  return center;
}

template< typename InputImageType, typename OutputImageType >
void
DefineOutputImageProperties( const typename InputImageType::ConstPointer& image,
                             typename OutputImageType::SpacingType&       outputSpacing,
                             typename OutputImageType::PointType&         outputOrigin,
                             typename OutputImageType::DirectionType&     outputDirection,
                             typename OutputImageType::SizeType&          outputSize,
                             typename InputImageType::PixelType&          minValue,
                             typename OutputImageType::PixelType&         defaultValue )
{
  using SizeValueType = typename InputImageType::SizeType::SizeValueType;
  const typename InputImageType::SpacingType   inputSpacing = image->GetSpacing();
  const typename InputImageType::PointType     inputOrigin = image->GetOrigin();
  const typename InputImageType::DirectionType inputDirection = image->GetDirection();
  const typename InputImageType::SizeType      inputSize = image->GetBufferedRegion().GetSize();

  const double scaleFixed = 0.9;
  for ( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
    {
      outputSpacing[i] = inputSpacing[i] * scaleFixed;
      outputOrigin[i] = inputOrigin[i] * scaleFixed;

      for ( unsigned int j = 0; j < InputImageType::ImageDimension; ++j )
        {
          outputDirection[i][j] = inputDirection[i][j];
        }
      outputSize[i] = itk::Math::Round< SizeValueType >( inputSize[i] * scaleFixed );
    }

  using MinimumMaximumImageCalculatorType = itk::MinimumMaximumImageCalculator< InputImageType >;
  auto calculator = MinimumMaximumImageCalculatorType::New();
  calculator->SetImage( image );
  calculator->ComputeMinimum();

  minValue = calculator->GetMinimum();
  defaultValue = minValue - 2;
}

template< typename InterpolatorType >
void
DefineInterpolator( typename InterpolatorType::Pointer& interpolator, const std::string& interpolatorName,
                    const unsigned int splineOrderInterpolator )
{
  // Interpolator typedefs
  using InputImageType = typename InterpolatorType::InputImageType;
  using CoordRepType = typename InterpolatorType::CoordRepType;
  using CoefficientType = CoordRepType;

  // Typedefs for all interpolators
  using NearestNeighborInterpolatorType = itk::NearestNeighborInterpolateImageFunction< InputImageType, CoordRepType >;
  using LinearInterpolatorType = itk::LinearInterpolateImageFunction< InputImageType, CoordRepType >;
  using BSplineInterpolatorType = itk::BSplineInterpolateImageFunction< InputImageType, CoordRepType, CoefficientType >;

  if ( interpolatorName == "Nearest" )
    {
      interpolator = NearestNeighborInterpolatorType::New();
    }
  else if ( interpolatorName == "Linear" )
    {
      interpolator = LinearInterpolatorType::New();
    }
  else if ( interpolatorName == "BSpline" )
    {
      auto bsplineInterpolator = BSplineInterpolatorType::New();
      bsplineInterpolator->SetSplineOrder( splineOrderInterpolator );
      interpolator = bsplineInterpolator;
    }
}

template< typename ExtrapolatorType >
void
DefineExtrapolator( typename ExtrapolatorType::Pointer& extrapolator, const std::string& extrapolatorName )
{
  // Extrapolator typedefs
  using InputImageType = typename ExtrapolatorType::InputImageType;
  using CoordRepType = typename ExtrapolatorType::CoordRepType;

  // Typedefs for all extrapolators
  using NearestNeighborExtrapolatorType = itk::NearestNeighborExtrapolateImageFunction< InputImageType, CoordRepType >;

  if ( extrapolatorName == "Nearest" )
    {
      extrapolator = NearestNeighborExtrapolatorType::New();
    }
}

template< typename AffineTransformType >
void
DefineAffineParameters( typename AffineTransformType::ParametersType& parameters )
{
  const unsigned int Dimension = AffineTransformType::InputSpaceDimension;

  // Setup parameters
  parameters.SetSize( Dimension * Dimension + Dimension );
  std::size_t par = 0;
  if ( Dimension == 2 )
    {
      const double matrix[] = {
        0.9, 0.1, // matrix part
        0.2, 1.1, // matrix part
        0.0, 0.0, // translation
      };

      for ( const double element : matrix )
        {
          parameters[par++] = element;
        }
    }
  else if ( Dimension == 3 )
    {
      const double matrix[] = {
        1.0,    -0.045, 0.02,  // matrix part
        0.0,    1.0,    0.0,   // matrix part
        -0.075, 0.09,   1.0,   // matrix part
        -3.02,  1.3,    -0.045 // translation
      };

      for ( const double element : matrix )
        {
          parameters[par++] = element;
        }
    }
}

template< typename TranslationTransformType >
void
DefineTranslationParameters( const std::size_t transformIndex,
                             typename TranslationTransformType::ParametersType& parameters )
{
  const std::size_t Dimension = TranslationTransformType::SpaceDimension;

  // Setup parameters
  parameters.SetSize( Dimension );
  for ( std::size_t i = 0; i < Dimension; ++i )
    {
      parameters[i] = ( i + 1.0 ) * transformIndex;
    }
}

template< typename BSplineTransformType >
void
DefineBSplineParameters( const std::size_t transformIndex, typename BSplineTransformType::ParametersType& parameters,
                         const typename BSplineTransformType::Pointer& transform )
{
  const unsigned int numberOfParameters = transform->GetNumberOfParameters();
  const unsigned int Dimension = BSplineTransformType::SpaceDimension;
  const unsigned int numberOfNodes = numberOfParameters / Dimension;

  parameters.SetSize( numberOfParameters );

  // Create a uniform distribution with seed based on the default + transform index
  std::default_random_engine                     randomEngine( 1234 + transformIndex );
  const std::uniform_real_distribution< double > randomNumberDistribution( -1.0, 1.0 );

  // Set the BSpline parameters from the uniform distribution
  for ( std::size_t n = 0; n < numberOfNodes * Dimension; ++n )
    {
      parameters[n] = randomNumberDistribution( randomEngine );
    }
}

template< typename EulerTransformType >
void
DefineEulerParameters( const std::size_t transformIndex, typename EulerTransformType::ParametersType& parameters )
{
  const std::size_t Dimension = EulerTransformType::InputSpaceDimension;

  // Setup parameters
  // 2D: angle 1, translation 2
  // 3D: 6 angle, translation 3
  parameters.SetSize( EulerTransformType::ParametersDimension );

  // Angle
  const double angle = transformIndex * -0.05;

  std::size_t par = 0;
  if ( Dimension == 2 )
    {
      // See implementation of Rigid2DTransform::SetParameters()
      parameters[0] = angle;
      ++par;
    }
  else if ( Dimension == 3 )
    {
      // See implementation of Rigid3DTransform::SetParameters()
      for ( std::size_t i = 0; i < 3; ++i )
        {
          parameters[par] = angle;
          ++par;
        }
    }

  for ( std::size_t i = 0; i < Dimension; ++i )
    {
      parameters[i + par] = ( i + 1.0 ) * transformIndex;
    }
}

template< typename SimilarityTransformType >
void
DefineSimilarityParameters( const std::size_t transformIndex,
                            typename SimilarityTransformType::ParametersType& parameters )
{
  const std::size_t Dimension = SimilarityTransformType::InputSpaceDimension;

  // Setup parameters
  // 2D: 2 translation, angle 1, scale 1
  // 3D: 3 translation, angle 3, scale 1
  parameters.SetSize( SimilarityTransformType::ParametersDimension );

  // Scale, Angle
  const double scale = ( transformIndex + 1.0 ) * 0.05 + 1.0;
  const double angle = transformIndex * -0.06;

  if ( Dimension == 2 )
    {
      // See implementation of Similarity2DTransform::SetParameters()
      parameters[0] = scale;
      parameters[1] = angle;
    }
  else if ( Dimension == 3 )
    {
      // See implementation of Similarity3DTransform::SetParameters()
      for ( std::size_t i = 0; i < Dimension; ++i )
        {
          parameters[i] = angle;
        }
      parameters[6] = scale;
    }

  // Translation
  for ( std::size_t i = 0; i < Dimension; ++i )
    {
      parameters[i + Dimension] = -1.0 * ( ( i + 1.0 ) * transformIndex );
    }
}

// This helper function completely sets the single transform
// We are supporting following ITK transforms:
// IdentityTransform, AffineTransform, TranslationTransform, BSplineTransform,
// EulerTransform, SimilarityTransform, CompositeTransform
template< typename TransformType, typename IdentityTransformType, typename AffineTransformType,
          typename TranslationTransformType, typename BSplineTransformType, typename EulerTransformType,
          typename SimilarityTransformType, typename CompositeTransformType, typename InputImageType >
void
SetTransform( const std::size_t transformIndex, const std::string& transformName,
              typename TransformType::Pointer& transform, typename CompositeTransformType::Pointer& compositeTransform,
              const typename InputImageType::ConstPointer& image )
{
  if ( transformName == "Identity" )
    {
      // Create Identity transform
      auto identityTransform = IdentityTransformType::New();
      if ( compositeTransform.IsNull() )
        {
          transform = identityTransform;
        }
      else
        {
          compositeTransform->AddTransform( identityTransform );
        }
    }
  else if ( transformName == "Affine" )
    {
      // Create Affine transform
      auto affineTransform = AffineTransformType::New();

      // Define and set affine parameters
      typename AffineTransformType::ParametersType parameters;
      DefineAffineParameters< AffineTransformType >( parameters );
      affineTransform->SetParameters( parameters );
      if ( compositeTransform.IsNull() )
        {
          transform = affineTransform;
        }
      else
        {
          compositeTransform->AddTransform( affineTransform );
        }
    }
  else if ( transformName == "Translation" )
    {
      // Create Translation transform
      auto translationTransform = TranslationTransformType::New();

      // Define and set translation parameters
      typename TranslationTransformType::ParametersType parameters;
      DefineTranslationParameters< TranslationTransformType >( transformIndex, parameters );
      translationTransform->SetParameters( parameters );
      if ( compositeTransform.IsNull() )
        {
          transform = translationTransform;
        }
      else
        {
          compositeTransform->AddTransform( translationTransform );
        }
    }
  else if ( transformName == "BSpline" )
    {
      const unsigned int                           Dimension = image->GetImageDimension();
      const typename InputImageType::SpacingType   inputSpacing = image->GetSpacing();
      const typename InputImageType::PointType     inputOrigin = image->GetOrigin();
      const typename InputImageType::DirectionType inputDirection = image->GetDirection();
      const typename InputImageType::SizeType      inputSize = image->GetBufferedRegion().GetSize();

      using MeshSizeType = typename BSplineTransformType::MeshSizeType;
      MeshSizeType gridSize;
      gridSize.Fill( 4 );

      using PhysicalDimensionsType = typename BSplineTransformType::PhysicalDimensionsType;
      PhysicalDimensionsType gridSpacing;
      for ( unsigned int i = 0; i < Dimension; ++i )
        {
          gridSpacing[i] = inputSpacing[i] * ( inputSize[i] - 1.0 );
        }

      // Create BSpline transform
      auto bsplineTransform = BSplineTransformType::New();

      // Set grid properties
      bsplineTransform->SetTransformDomainOrigin( inputOrigin );
      bsplineTransform->SetTransformDomainDirection( inputDirection );
      bsplineTransform->SetTransformDomainPhysicalDimensions( gridSpacing );
      bsplineTransform->SetTransformDomainMeshSize( gridSize );

      // Define and set b-spline parameters
      typename BSplineTransformType::ParametersType parameters;
      DefineBSplineParameters< BSplineTransformType >( transformIndex, parameters, bsplineTransform );
      bsplineTransform->SetParameters( parameters );
      if ( compositeTransform.IsNull() )
        {
          transform = bsplineTransform;
        }
      else
        {
          compositeTransform->AddTransform( bsplineTransform );
        }
    }
  else if ( transformName == "Euler" )
    {
      // Create Euler transform
      auto eulerTransform = EulerTransformType::New();

      // Compute and set center
      const typename InputImageType::PointType center = ComputeCenterOfTheImage< InputImageType >( image );
      eulerTransform->SetCenter( center );

      // Define and set euler parameters
      typename EulerTransformType::ParametersType parameters;
      DefineEulerParameters< EulerTransformType >( transformIndex, parameters );
      eulerTransform->SetParameters( parameters );
      if ( compositeTransform.IsNull() )
        {
          transform = eulerTransform;
        }
      else
        {
          compositeTransform->AddTransform( eulerTransform );
        }
    }
  else if ( transformName == "Similarity" )
    {
      // Create Similarity transform
      auto similarityTransform = SimilarityTransformType::New();

      // Compute and set center
      const typename InputImageType::PointType center = ComputeCenterOfTheImage< InputImageType >( image );
      similarityTransform->SetCenter( center );

      // Define and set similarity parameters
      typename SimilarityTransformType::ParametersType parameters;
      DefineSimilarityParameters< SimilarityTransformType >( transformIndex, parameters );
      similarityTransform->SetParameters( parameters );
      if ( compositeTransform.IsNull() )
        {
          transform = similarityTransform;
        }
      else
        {
          compositeTransform->AddTransform( similarityTransform );
        }
    }
}

// This helper function completely defines the transform[s]
// We are supporting following ITK transforms:
// IdentityTransform, AffineTransform, TranslationTransform, BSplineTransform,
// EulerTransform, SimilarityTransform, CompositeTransform
template< typename TransformType, typename IdentityTransformType, typename AffineTransformType,
          typename TranslationTransformType, typename BSplineTransformType, typename EulerTransformType,
          typename SimilarityTransformType, typename CompositeTransformType, typename InputImageType >
void
DefineTransform( typename TransformType::Pointer& transform, const Parameters& parameters,
                 const typename InputImageType::ConstPointer& image )
{
  if ( !parameters.useCompositeTransform )
    {
      if ( !parameters.transforms.empty() )
        {
          typename CompositeTransformType::Pointer dummy;
          SetTransform< TransformType,
                        IdentityTransformType,
                        AffineTransformType,
                        TranslationTransformType,
                        BSplineTransformType,
                        EulerTransformType,
                        SimilarityTransformType,
                        CompositeTransformType,
                        InputImageType >
            ( 0, parameters.transforms[0], transform, dummy, image );
        }
    }
  else
    {
      auto compositeTransform = CompositeTransformType::New();
      transform = compositeTransform;

      for ( std::size_t i = 0; i < parameters.transforms.size(); ++i )
        {
          SetTransform< TransformType,
                        IdentityTransformType,
                        AffineTransformType,
                        TranslationTransformType,
                        BSplineTransformType,
                        EulerTransformType,
                        SimilarityTransformType,
                        CompositeTransformType,
                        InputImageType >
            ( i, parameters.transforms[i], transform, compositeTransform, image );
        }
    }
}

// Check for transforms that support only 2D/3D not 1D
bool
HasNotSupportedTransform1D( const Parameters& _parameters, const unsigned int dimension )
{
  bool notSupportFor1D = false;
  if ( dimension == 1 )
    {
      for ( const auto& transform : _parameters.transforms )
        {
          if ( transform == "Euler" || transform == "Similarity" )
            {
              notSupportFor1D = true;
              break;
            }
        }
    }

  return notSupportFor1D;
}

// Check if the required arguments are given to the benchmark
bool
ValidateArguments( const Parameters& _parameters )
{
  // Check the input image sizes if provided
  bool inputSizesProvidedAndValid = false;
  if ( _parameters.imageSizes.size() > 3 )
    {
      std::cerr << "ERROR: Only 1D/2D/3D images are supported with \"-is\" option." << std::endl;
      return false;
    }

  for ( auto i : _parameters.imageSizes )
    {
      if ( i > 0 )
        {
          inputSizesProvidedAndValid = true;
        }
      else
        {
          std::cerr << "ERROR: You should specify valid images sizes with \"-is dim1 [dim2] [dim3]\"" << std::endl;
          return false;
        }
    }

  // Check that either input file or images sizes are provided for the benchmark
  if ( inputSizesProvidedAndValid && !_parameters.inputFileName.empty() )
    {
      std::cerr << "ERROR: You should specify input file with \"-in\" or images sizes with \"-is dim1 [dim2] [dim3]\""
                   "\nYou could not use both \"-in\" and \"-is\" together."
                << std::endl;
      return false;
    }

  // Check the iterations
  if ( _parameters.iterations < 1 )
    {
      std::cerr << "ERROR: \"iterations\" parameter should be more or equal one." << std::endl;
      return false;
    }

  // Check the interpolator
  const std::vector<std::string> validInterpolators = { "Nearest", "Linear", "BSpline" };
  if ( !IsStringInVector( _parameters.interpolator, validInterpolators ) )
    {
      std::cerr << "ERROR: interpolator \"-i\" should be one of {Nearest, Linear, BSpline}." << std::endl;
      return false;
    }

  // Check the extrapolator
  if ( !_parameters.extrapolator.empty() )
    {
      if ( _parameters.extrapolator != "Nearest" )
        {
          std::cerr << "ERROR: extrapolator \"-e\" should only be {Nearest}." << std::endl;
          return false;
        }
    }

  // Check for the supported transforms
  const std::vector< std::string > validTransforms = { "Identity", "Affine", "Translation",
                                                       "BSpline",  "Euler",  "Similarity" };
  if ( !AreAllStringsInVector( _parameters.transforms, validTransforms ) )
    {
      std::cerr << "ERROR: \"transforms\" should be one of "
                << "{Identity, Affine, Translation, BSpline, Euler, Similarity}"
                << " or combination of them." << std::endl;
      return false;
    }

  // Check for the supported transforms precision
  const std::vector< std::string > validTransformPrecisions = { "float", "double" };
  if ( !IsStringInVector( _parameters.transformsPrecision, validTransformPrecisions ) )
    {
      std::cerr << "ERROR: transforms precision \"-tp\" should be one of {float, double}." << std::endl;
      return false;
    }

  return true;
}

template< typename TransformPrecisionType, typename EulerTransformType, typename SimilarityTransformType,
          class InputImageType, typename OutputImageType = InputImageType >
int
ProcessImage( const Parameters& _parameters )
{
  // Setting the threads
  if ( _parameters.threads > 0 )
    {
      itk::MultiThreader::SetGlobalDefaultNumberOfThreads( _parameters.threads );
    }

  // Input image dimension
  const unsigned int ImageDim = InputImageType::ImageDimension;

  // Interpolator, extrapolator precision typedefs
  using InterpolatorPrecisionType = TransformPrecisionType;
  using ExtrapolatorPrecisionType = TransformPrecisionType;

  // Resample filter typedef
  using ResampleFilterType = itk::ResampleImageFilter< InputImageType, OutputImageType, InterpolatorPrecisionType >;

  // Transform typedefs
  using TransformType = itk::Transform< TransformPrecisionType, ImageDim, ImageDim >;
  using IdentityTransformType = itk::IdentityTransform< TransformPrecisionType, ImageDim >;
  using AffineTransformType = itk::AffineTransform< TransformPrecisionType, ImageDim >;
  using TranslationTransformType = itk::TranslationTransform< TransformPrecisionType, ImageDim >;
  using BSplineTransformType = itk::BSplineTransform< TransformPrecisionType, ImageDim, 3 >;
  using CompositeTransformType = itk::CompositeTransform< TransformPrecisionType, ImageDim >;

  // Interpolator, extrapolator typedefs
  using InterpolatorType = itk::InterpolateImageFunction< InputImageType, InterpolatorPrecisionType >;
  using ExtrapolatorType = itk::ExtrapolateImageFunction< InputImageType, ExtrapolatorPrecisionType >;

  // Typedefs reader, writer
  using ReaderType = itk::ImageFileReader< InputImageType >;
  using InputWriterType = itk::ImageFileWriter< InputImageType >;
  using OutputWriterType = itk::ImageFileWriter< OutputImageType >;

  // Input, output image properties
  typename InputImageType::RegionType     inputRegion;
  typename OutputImageType::SpacingType   outputSpacing;
  typename OutputImageType::PointType     outputOrigin;
  typename OutputImageType::DirectionType outputDirection;
  typename OutputImageType::SizeType      outputSize;

  // Extra parameters
  auto minValue = itk::NumericTraits< typename InputImageType::PixelType >::Zero;
  auto defaultValue = itk::NumericTraits< typename OutputImageType::PixelType >::Zero;

  // Input image size
  using SizeType = itk::Size< ImageDim >;
  SizeType imageSize;
  imageSize.Fill( 0 );

  // Resample main components
  typename InterpolatorType::Pointer interpolator;
  typename ExtrapolatorType::Pointer extrapolator;
  typename TransformType::Pointer    transform;

  // Read or create the image
  typename InputImageType::ConstPointer inputImage = nullptr;
  if ( !_parameters.inputFileName.empty() )
    {
      auto reader = ReaderType::New();
      reader->SetFileName( _parameters.inputFileName );
      try
        {
          reader->Update();
        }
      catch ( const itk::ExceptionObject& exceptionObject )
        {
          std::cerr << "Caught ITK exception during reader->Update(): " << exceptionObject << std::endl;
          return EXIT_FAILURE;
        }

      inputImage = reader->GetOutput();
    }
  else
    {
      inputImage = CreateInputImage< InputImageType >( _parameters.imageSizes );
    }

  imageSize = inputImage->GetBufferedRegion().GetSize();
  inputRegion = inputImage->GetBufferedRegion();

  // Get all properties we need for the output image
  DefineOutputImageProperties< InputImageType, OutputImageType >(
    inputImage, outputSpacing, outputOrigin, outputDirection, outputSize, minValue, defaultValue );

  // Create Resample filter
  auto resample = ResampleFilterType::New();

  resample->SetDefaultPixelValue( defaultValue );
  resample->SetOutputSpacing( outputSpacing );
  resample->SetOutputOrigin( outputOrigin );
  resample->SetOutputDirection( outputDirection );
  resample->SetSize( outputSize );
  resample->SetOutputStartIndex( inputRegion.GetIndex() );

  // Construct, select and setup transform
  DefineTransform< TransformType,
                   IdentityTransformType,
                   AffineTransformType,
                   TranslationTransformType,
                   BSplineTransformType,
                   EulerTransformType,
                   SimilarityTransformType,
                   CompositeTransformType,
                   InputImageType >( transform, _parameters, inputImage );

  // Create interpolator here
  DefineInterpolator< InterpolatorType >( interpolator, _parameters.interpolator, _parameters.splineOrderInterpolator );

  // Create extrapolator here
  DefineExtrapolator< ExtrapolatorType >( extrapolator, _parameters.extrapolator );

  // Print resample execution info
  std::cout << "Benchmarking Resample filter with " << itk::MultiThreaderBase::GetGlobalDefaultNumberOfThreads()
            << " threads" << std::endl;
  std::cout << "Image size: " << imageSize << std::endl;
  std::cout << "Interpolator type: " << interpolator->GetNameOfClass() << std::endl;
  if ( extrapolator.IsNull() )
    std::cout << "Extrapolator type: None" << std::endl;
  else
    std::cout << "Extrapolator type: " << extrapolator->GetNameOfClass() << std::endl;

  std::cout << "Transform type: " << GetTransformName< TransformType, InputImageType >( transform ) << std::endl;

  // Set up the resample with input, transforms[s], interpolator and extrapolator
  try
    {
      resample->SetInput( inputImage );
      resample->SetTransform( transform );
      resample->SetInterpolator( interpolator );
      resample->SetExtrapolator( extrapolator );
    }
  catch ( const itk::ExceptionObject& exceptionObject )
    {
      std::cerr << "Caught ITK exception during initialization of the Resample filter: " << exceptionObject << std::endl;
      return EXIT_FAILURE;
    }

  itk::HighPriorityRealTimeProbesCollector collector;

  for ( int i = 0; i < _parameters.iterations; ++i )
    {
      try
        {
          collector.Start( "Resample" );
          resample->Update();
          collector.Stop( "Resample" );
        }
      catch ( const itk::ExceptionObject& exceptionObject )
        {
          std::cerr << "Caught ITK exception during Resample filter Update() call: " << exceptionObject << std::endl;
          return EXIT_FAILURE;
        }

      // Modify the filter, only not the last iteration
      if ( i != _parameters.iterations - 1 )
        {
          resample->Modified();
        }
    }

  WriteExpandedReport( _parameters.timingsFileName, collector, true, true, false );

  // Write the input here, helps in case of the generated image see case (b)
  const bool writeInputImage = _parameters.outputFileNames.size() > 1;
  if ( writeInputImage )
    {
      // Write the input image
      auto writer = InputWriterType::New();
      writer->SetFileName( _parameters.outputFileNames[0] );
      writer->SetInput( inputImage );
      writer->Update();
    }

  // Write the output of the resample filter
  if ( !_parameters.outputFileNames.empty() )
    {
      const std::string outputFileName =
        writeInputImage ? _parameters.outputFileNames[1] : _parameters.outputFileNames[0];
      auto writer = OutputWriterType::New();
      writer->SetFileName( outputFileName );
      writer->SetInput( resample->GetOutput() );
      writer->Update();
    }

  return EXIT_SUCCESS;
}

template< typename TransformPrecisionType >
int
ProcessImageOfDimension(const std::size_t Dimension, const Parameters& parameters)
{
  // Dummy Euler1DTransformType and Similarity1DTransformType for compiler
  using Euler1DTransformType = itk::MatrixOffsetTransformBase< TransformPrecisionType, 1, 1 >;
  using Similarity1DTransformType = itk::MatrixOffsetTransformBase< TransformPrecisionType, 1, 1 >;

  // Typedefs for Euler2D/3D Similarity2D/3D
  using Euler2DTransformType = itk::Euler2DTransform< TransformPrecisionType >;
  using Similarity2DTransformType = itk::Similarity2DTransform< TransformPrecisionType >;
  using Euler3DTransformType = itk::Euler3DTransform< TransformPrecisionType >;
  using Similarity3DTransformType = itk::Similarity3DTransform< TransformPrecisionType >;

  switch (Dimension)
  {
  case 1: // 1D
    return ProcessImage< TransformPrecisionType,
      Euler1DTransformType,
      Similarity1DTransformType,
      itk::Image< short, 1 > >(parameters);
  case 2: // 2D
    return ProcessImage< TransformPrecisionType,
      Euler2DTransformType,
      Similarity2DTransformType,
      itk::Image< short, 2 > >(parameters);
  case 3: // 3D
    return ProcessImage< TransformPrecisionType,
      Euler3DTransformType,
      Similarity3DTransformType,
      itk::Image< short, 3 > >(parameters);
  }
  return EXIT_FAILURE;
}

} // End of namespace

void AdjustTransformParameters(Parameters &parameters)
{
  // If no transform are provided then set to Identity as default, same as Resample constructor does
  if (parameters.transforms.empty())
  {
    parameters.transforms = { "Identity" };
  }
  else if (parameters.transforms.size() > 1)
  {
    // Auto enable the CompositeTransform (itk::CompositeTransform) for multiple transforms
    parameters.useCompositeTransform = true;
  }
}

// This test performs benchmarking of the ITK ResampleImageFilter.
// The benchmarking has been designed for two modes:
//  a. Execute on the input image from file, when option "-in" "input_image" is provided, or
//  b. Execute on the test gradient pattern image when option "-is" "dim1" ["dim2"] ["dim3"] is provided.
//     The dimension (1D/2D/3D) of the input image will be auto detected.
// Option (a) required to maintain maximum compatibility with Insight Journal article:
// http://www.insight-journal.org/browse/publication/884
// Option (b) does not required any input file to be provided or downloaded. The test gradient pattern will
// be created as an input. That makes execution of the benchmark easy.
// Both options "-in" and "-is" should be maintained for this benchmark.
//
// The resample filter takes an input image and produces an output image to "-out" "output_image".
// Optionally the input image could be also saved when two files are provided "-out" "input_image" "output_image".
// The command line option "-tf" "timings_file.json" is used for recording build information
// and timing of the benchmark to JSON file.
// The command line option "-iterations" controls how many times resample filter will execute for real benchmarking
// to compute the total(s), min(s), mean(s), max(s) and stddev(s) execution's.
// The command line option "-threads" controls default global number of threads used by any filter in ITK.
// For example, when BSpline interpolator is used "-i" "BSpline", the BSplineDecompositionImageFilter
// will be also executed with number of threads provided by option "-threads".
//
// The following ITK interpolations are supported in this benchmark:
// itk::NearestNeighborInterpolateImageFunction
// itk::LinearInterpolateImageFunction
// itk::BSplineInterpolateImageFunction
//
// Command line argument "-i" "Linear" ["Nearest"] ["BSpline"]
// Command line argument "-soi" controls spline order interpolator for the BSpline interpolator.
// For "-i" "BSpline" the 0th - 5th order splines are supported, the default 3rd.
//
// The following ITK extrapolations are supported in this benchmark:
// itk::NearestNeighborExtrapolateImageFunction
//
// Command line argument "-e" "Nearest"
//
// The following ITK transforms are supported in this benchmark:
// itk::CompositeTransform
// itk::IdentityTransform
// itk::AffineTransform
// itk::BSplineTransform
// itk::Euler2DTransform
// itk::Euler3DTransform
// itk::Similarity2DTransform
// itk::Similarity3DTransform
// itk::TranslationTransform
//
// Command line argument "-t" "Affine" ["BSpline"] ["Euler"] ["Similarity"] ["Translation"] ["Identity"]
// Composite transforms will be used when multiple transforms are provided, example "-t" "Affine" "BSpline"
// Single transforms could also be executed as composite transform with option "-c", example "-t" "Affine" "-c"
// All transforms settings are designed with attention to produce non blank output image during resampling (see (a)).
// The default interpolator is "Linear", default transform is "Identity", same as defaults for the ResampleImageFilter.
// Command line argument "-tp" controls the precision of the transforms, both 'float' and 'double' are supported,
// default 'float'.
//
// Examples of Resample benchmark execution:
// 1. Minimalistic execution of Resample benchmark with 2D image of size [200, 240].
//  ResampleBenchmark -is 200 240
// 2. Resample 1D image of size [8192] with BSpline interpolator (order 2) and Affine transform.
//  ResampleBenchmark -is 8192 -i BSpline -soi 2 -t Affine -out output.mha -tf tf.json
// 3. Resample 2D image of size [256, 240] with Resample defaults (Linear interpolator and Identity transform).
//  ResampleBenchmark -is 256 240 -out output.mha -tf tf.json
// 4. Resample 2D image of size [1800, 2400] with Nearest interpolator and Affine transform.
//  ResampleBenchmark -is 1800 2400 -i Nearest -t Affine -out output.mha -tf tf.json
// 5. Resample 3D image of size [512, 512, 256] with Nearest interpolator, Affine and BSpline transforms.
//  ResampleBenchmark -is 512 512 256 -i Nearest -t Affine BSpline -out output.mha -tf tf.json
// 6. Resample 3D image of size [512, 512, 256] with BSpline interpolator (order 5),
// Translation, Affine, BSpline, Euler, Similarity transforms (in double precision), save the input image and perform
// benchmarking 10 times.
//  ResampleBenchmark -is 512 512 256 -i BSpline -soi 5 -e Nearest -t  Translation Affine BSpline Euler Similarity
//                    -tp double -out input.mha output.mha -iterations 10 -tf tf.json
// 7. Similar to Insight Journal article http://www.insight-journal.org/browse/publication/884 execution:
// Resample 3D image from file with BSpline interpolator, Affine and BSpline transforms, save the input image.
// ResampleBenchmark -in "insight_journal\data\image-256x256x256-3D.mha" -i BSpline -t Affine BSpline
//                   -out input.mha output.mha -tf tf.json
//
int
main( int argc, char* argv[] )
{
  // Define the input arguments for the benchmark
  itksys::CommandLineArguments commandLineArguments;
  commandLineArguments.SetLineLength( 160 );
  commandLineArguments.Initialize( argc, argv );

  // Create parameters class to store the command line arguments
  Parameters parameters;

  // Benchmark main settings
  commandLineArguments.AddArgument( "-tf", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.timingsFileName,
    "timings file name" );
  commandLineArguments.AddArgument( "-iterations", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.iterations,
    "controls how many times filter will execute for benchmarking. default 1" );
  commandLineArguments.AddArgument( "-threads", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.threads,
    "number of threads. default maximum" );

  // Input, output images
  commandLineArguments.AddArgument( "-in", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.inputFileName,
    "input file name" );
  commandLineArguments.AddArgument( "-is", itksys::CommandLineArguments::MULTI_ARGUMENT, &parameters.imageSizes,
    "input image sizes, dim1, [dim2], [dim3]" );
  commandLineArguments.AddArgument( "-out", itksys::CommandLineArguments::MULTI_ARGUMENT, &parameters.outputFileNames,
    "output file name[s]. If two files are provided, then input image are saved as well." );

  // Interpolator, extrapolator and transforms settings
  commandLineArguments.AddArgument( "-i", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.interpolator,
    "interpolator, one of {Nearest, Linear, BSpline}. default Linear" );
  commandLineArguments.AddArgument( "-soi", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.splineOrderInterpolator,
    "spline order interpolator for the BSpline interpolator (0th - 5th order splines are supported). default 3 " );
  commandLineArguments.AddArgument( "-e", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.extrapolator,
    "extrapolator, only {Nearest}, default None" );
  commandLineArguments.AddArgument( "-c", itksys::CommandLineArguments::NO_ARGUMENT, &parameters.useCompositeTransform,
    "use composite transform, also could be used with single transform. default false" );
  commandLineArguments.AddArgument( "-t", itksys::CommandLineArguments::MULTI_ARGUMENT, &parameters.transforms,
    "transforms, one of {Identity, Affine, Translation, BSpline, Euler, Similarity}" );
  commandLineArguments.AddArgument( "-tp", itksys::CommandLineArguments::SPACE_ARGUMENT, &parameters.transformsPrecision,
    "transforms precision, one of {float, double}. default float" );

  // Parse the command line arguments
  if ( !commandLineArguments.Parse() )
    {
      std::cerr << commandLineArguments.GetHelp() << std::endl;
      std::cerr << "ERROR: Problem parsing Resample benchmark arguments" << std::endl;
      return EXIT_FAILURE;
    }

  // Replace the __DATESTAMP__ with real date and time
  parameters.timingsFileName = ReplaceOccurrence( parameters.timingsFileName, "__DATESTAMP__", PerfDateStamp() );

  // Perform the validation of the command arguments provided by the user or CMake
  if ( !ValidateArguments( parameters ) )
    {
      return EXIT_FAILURE;
    }

  // Adjust the parameters based on the command arguments provided
  AdjustTransformParameters( parameters );

  // We ether going to read the input image and perform the benchmark when option '-in file' is provided.
  // Or we are going to create the pattern of the input image when option '-is 512 512 256' provided.
  int  result = EXIT_SUCCESS;
  if ( !parameters.inputFileName.empty() )
    {
      // Determine the input image properties.
      std::string                 ComponentType = "short";
      std::string                 PixelType; // we don't use this
      unsigned int                Dimension = 2;
      unsigned int                NumberOfComponents = 1;
      std::vector< unsigned int > imagesize( Dimension, 0 );
      const bool                  retgip = GetImageProperties(
        parameters.inputFileName, PixelType, ComponentType, Dimension, NumberOfComponents, imagesize );

      if ( !retgip )
        {
          return EXIT_FAILURE;
        }

      // Let the user overrule this
      if ( NumberOfComponents > 1 )
        {
          std::cerr << "ERROR: The NumberOfComponents is larger than 1.\n"
                       "Vector images are not supported by this benchmark."
                    << std::endl;
          return EXIT_FAILURE;
        }

      // Check for the not supported transforms in 1D
      if ( HasNotSupportedTransform1D( parameters, Dimension ) )
        {
          std::cerr
            << "ERROR: The transforms has one that not supported in 1D. Check if you set Euler or Similarity in 1D."
            << std::endl;
          return EXIT_FAILURE;
        }

      try
        {
          if ( ComponentType == "short" )
            {
              // Run the benchmark for the input image from file with transforms precision
              if ( parameters.transformsPrecision == "float" )
                {
                  result = ProcessImageOfDimension< float >( Dimension, parameters );
                }
              else if ( parameters.transformsPrecision == "double" )
                {
                  result = ProcessImageOfDimension< double >( Dimension, parameters );
                }
            }
          else
            {
              std::cerr << "ERROR: This combination of pixeltype and dimension is not supported by this benchmark.\n"
                           "pixel (component) type = "
                        << ComponentType << " ; dimension = " << Dimension << std::endl;
              return EXIT_FAILURE;
            }
        }
      catch ( const itk::ExceptionObject& exceptionObject )
        {
          std::cerr << "Caught ITK exception: " << exceptionObject << std::endl;
          return EXIT_FAILURE;
        }
    }
  else
    {
      try
        {
          const std::size_t Dimension = parameters.imageSizes.size();

          // Check for the not supported transforms in 1D
          if ( HasNotSupportedTransform1D( parameters, Dimension ) )
            {
              std::cerr
                << "ERROR: The transforms has one that not supported in 1D. Check if you set Euler or Similarity in 1D."
                << std::endl;
              return EXIT_FAILURE;
            }

          // Run the benchmark for the created input image with transforms precision
          if ( parameters.transformsPrecision == "float" )
            {
              result = ProcessImageOfDimension< float >( Dimension, parameters );
            }
          else if ( parameters.transformsPrecision == "double" )
            {
              result = ProcessImageOfDimension< double >( Dimension, parameters );
            }
        }
      catch ( const itk::ExceptionObject& exceptionObject )
        {
          std::cerr << "Caught ITK exception: " << exceptionObject << std::endl;
          return EXIT_FAILURE;
        }
    }

  // End of the benchmark.
  return result;
}
