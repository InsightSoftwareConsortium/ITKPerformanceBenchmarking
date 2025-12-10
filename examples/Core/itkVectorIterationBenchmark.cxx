/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// This Benchmark time the performance of different iteration loop while converting between various vector image and
// pixel types.


#include <iostream>
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkVector.h"
#include "itkRGBPixel.h"
#include "itkImageRegionRange.h"
#include "itkImageScanlineIterator.h"
#include "itkImageScanlineConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"
#include "itkNumericTraits.h"
#include <iomanip>
#include <fstream>


namespace
{
template <typename T>
static constexpr bool isVariableLengthVector = std::is_same_v<T, itk::VariableLengthVector<typename T::ValueType>>;
}

// Helper function to initialize an image with random values
template <typename TImage>
typename TImage::Pointer
CreateAndInitializeImage(const typename TImage::SizeType & size, unsigned int numberOfComponentsPerPixel = 0)
{
  auto                        image = TImage::New();
  typename TImage::RegionType region{ size };
  image->SetRegions(region);
  if (numberOfComponentsPerPixel > 0)
  {
    image->SetNumberOfComponentsPerPixel(numberOfComponentsPerPixel);
  }
  image->Allocate();

  // Initialize with simple pattern (pixel index-based)
  using PixelType = typename TImage::PixelType;
  unsigned int count = 0;

  const unsigned int length = image->GetNumberOfComponentsPerPixel();

  itk::ImageRegionIterator<TImage> it(image, region);
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
  {
    PixelType pixel{ it.Get() };
    for (unsigned int k = 0; k < length; ++k)
    {
      pixel[k] = static_cast<typename itk::NumericTraits<PixelType>::ValueType>(count + k);
    }
    it.Set(pixel);
    ++count;
  }

  return image;
}


// Method 1: ImageScanlineIterator approach
template <typename TInputImage, typename TOutputImage>
void
CopyScanlineIterator(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;
  using ImageScanlineConstIterator = itk::ImageScanlineConstIterator<TInputImage>;
  using ImageScanlineIterator = itk::ImageScanlineIterator<TOutputImage>;

  const typename TOutputImage::RegionType outputRegion = outputPtr->GetRequestedRegion();
  typename TInputImage::RegionType        inputRegion = outputRegion;

  ImageScanlineConstIterator inputIt(inputPtr, inputRegion);
  ImageScanlineIterator      outputIt(outputPtr, outputRegion);

  const unsigned int componentsPerPixel = inputPtr->GetNumberOfComponentsPerPixel();
  while (!inputIt.IsAtEnd())
  {
    while (!inputIt.IsAtEndOfLine())
    {
      const InputPixelType & inputPixel = inputIt.Get();

      if constexpr (isVariableLengthVector<OutputPixelType>)
      {
        OutputPixelType value(outputIt.Get());
        for (unsigned int k = 0; k < componentsPerPixel; ++k)
        {
          value[k] = static_cast<typename OutputPixelType::ValueType>(inputPixel[k]);
        }
      }
      else
      {
        OutputPixelType value;
        for (unsigned int k = 0; k < componentsPerPixel; ++k)
        {
          value[k] = static_cast<typename OutputPixelType::ValueType>(inputPixel[k]);
        }
        outputIt.Set(value);
      }

      ++inputIt;
      ++outputIt;
    }
    inputIt.NextLine();
    outputIt.NextLine();
  }
}


// Method 1b: ImageScanlineIterator approach using NumericTraits::GetLength()
template <typename TInputImage, typename TOutputImage>
void
CopyScanlineIteratorNumericTraits(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;
  using ImageScanlineConstIterator = itk::ImageScanlineConstIterator<TInputImage>;
  using ImageScanlineIterator = itk::ImageScanlineIterator<TOutputImage>;

  const typename TOutputImage::RegionType outputRegion = outputPtr->GetRequestedRegion();
  typename TInputImage::RegionType        inputRegion = outputRegion;

  ImageScanlineConstIterator inputIt(inputPtr, inputRegion);
  ImageScanlineIterator      outputIt(outputPtr, outputRegion);

  unsigned int componentsPerPixel = itk::NumericTraits<OutputPixelType>::GetLength(outputIt.Get());
  while (!inputIt.IsAtEnd())
  {
    while (!inputIt.IsAtEndOfLine())
    {
      const InputPixelType & inputPixel = inputIt.Get();

      if constexpr (isVariableLengthVector<OutputPixelType>)
      {
        OutputPixelType value(outputIt.Get());
        for (unsigned int k = 0; k < componentsPerPixel; ++k)
        {
          value[k] = static_cast<typename OutputPixelType::ValueType>(inputPixel[k]);
        }
      }
      else
      {
        OutputPixelType value;
        for (unsigned int k = 0; k < componentsPerPixel; ++k)
        {
          value[k] = static_cast<typename OutputPixelType::ValueType>(inputPixel[k]);
        }
        outputIt.Set(value);
      }

      ++inputIt;
      ++outputIt;
    }
    inputIt.NextLine();
    outputIt.NextLine();
  }
}


// Method 2: ImageRegionRange approach
template <typename TInputImage, typename TOutputImage>
void
CopyImageRegionRange(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;

  const typename TOutputImage::RegionType outputRegion = outputPtr->GetRequestedRegion();
  typename TInputImage::RegionType        inputRegion = outputRegion;

  auto inputRange = itk::ImageRegionRange<const TInputImage>(*inputPtr, inputRegion);
  auto outputRange = itk::ImageRegionRange<TOutputImage>(*outputPtr, outputRegion);

  auto       inputIt = inputRange.begin();
  auto       outputIt = outputRange.begin();
  const auto inputEnd = inputRange.end();

  const unsigned int componentsPerPixel = inputPtr->GetNumberOfComponentsPerPixel();
  while (inputIt != inputEnd)
  {
    const InputPixelType & inputPixel = *inputIt;
    std::conditional_t<isVariableLengthVector<OutputPixelType>, OutputPixelType, OutputPixelType &> outputPixel{
      *outputIt
    };
    for (unsigned int k = 0; k < componentsPerPixel; ++k)
    {
      outputPixel[k] = static_cast<typename OutputPixelType::ValueType>(inputPixel[k]);
    }

    ++inputIt;
    ++outputIt;
  }
}


// Method 2b: ImageRegionRange approach using NumericTraits::GetLength()
template <typename TInputImage, typename TOutputImage>
void
CopyImageRegionRangeNumericTraits(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;

  const typename TOutputImage::RegionType outputRegion = outputPtr->GetRequestedRegion();
  typename TInputImage::RegionType        inputRegion = outputRegion;

  auto inputRange = itk::ImageRegionRange<const TInputImage>(*inputPtr, inputRegion);
  auto outputRange = itk::ImageRegionRange<TOutputImage>(*outputPtr, outputRegion);

  auto       inputIt = inputRange.begin();
  auto       outputIt = outputRange.begin();
  const auto inputEnd = inputRange.end();

  const unsigned int componentsPerPixel = itk::NumericTraits<OutputPixelType>::GetLength(*outputIt);
  while (inputIt != inputEnd)
  {
    const InputPixelType & inputPixel = *inputIt;
    std::conditional_t<isVariableLengthVector<OutputPixelType>, OutputPixelType, OutputPixelType &> outputPixel{
      *outputIt
    };
    for (unsigned int k = 0; k < componentsPerPixel; ++k)
    {
      outputPixel[k] = static_cast<typename OutputPixelType::ValueType>(inputPixel[k]);
    }
    ++inputIt;
    ++outputIt;
  }
}


// Method 2c: ImageRegionRange and a Range-based loop using NumericTraits::GetLength()
template <typename TInputImage, typename TOutputImage>
void
CopyImageRegionRangeNumericTraitsAsRange(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;

  const typename TOutputImage::RegionType outputRegion = outputPtr->GetRequestedRegion();
  typename TInputImage::RegionType        inputRegion = outputRegion;

  auto outputRange = itk::ImageRegionRange<TOutputImage>(*outputPtr, outputRegion);
  auto outputIt = outputRange.begin();

  const unsigned int componentsPerPixel = itk::NumericTraits<OutputPixelType>::GetLength(*outputIt);
  for (const InputPixelType & inputPixel : itk::ImageRegionRange<const TInputImage>(*inputPtr, inputRegion))
  {
    std::conditional_t<isVariableLengthVector<OutputPixelType>, OutputPixelType, OutputPixelType &> outputPixel{
      *outputIt
    };
    for (unsigned int k = 0; k < componentsPerPixel; ++k)
    {
      outputPixel[k] = static_cast<typename OutputPixelType::ValueType>(inputPixel[k]);
    }
    ++outputIt;
  }
}


// Helper function to time a single method
template <typename TInputImage, typename TOutputImage, typename TCopyFunction>
void
TimeMethod(itk::HighPriorityRealTimeProbesCollector & collector,
           const std::string &                        methodName,
           TCopyFunction                              copyFunc,
           const TInputImage *                        inputImage,
           typename TOutputImage::Pointer &           outputImage,
           int                                        iterations)
{
  // Allocate output image
  outputImage = TOutputImage::New();
  outputImage->SetRegions(inputImage->GetLargestPossibleRegion());
  outputImage->SetNumberOfComponentsPerPixel(inputImage->GetNumberOfComponentsPerPixel());
  outputImage->Allocate();

  // Warm-up run
  copyFunc(inputImage, outputImage.GetPointer());

  // Timed runs
  for (int ii = 0; ii < iterations; ++ii)
  {
    collector.Start(methodName.c_str());
    copyFunc(inputImage, outputImage.GetPointer());
    collector.Stop(methodName.c_str());
  }
}


// Performance testing function
template <typename TInputImage, typename TOutputImage>
void
TimeIterationMethods(itk::HighPriorityRealTimeProbesCollector & collector,
                     const typename TInputImage::SizeType &     size,
                     const std::string &                        description,
                     int                                        iterations)
{

  // Create and initialize input image
  auto inputImage = CreateAndInitializeImage<TInputImage>(size, 3);

  typename TOutputImage::Pointer referenceImage;
  typename TOutputImage::Pointer outputImage;

  // Test Method 1: Scanline Iterator - serves as reference
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-Scanline",
                                        CopyScanlineIterator<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 2: ImageRegionRange
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-Range",
                                        CopyImageRegionRange<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 1b: Scanline Iterator with NumericTraits
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-Scanline NT",
                                        CopyScanlineIteratorNumericTraits<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 2b: ImageRegionRange with NumericTraits
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-Range NT",
                                        CopyImageRegionRangeNumericTraits<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 2c: ImageRegionRange with NumericTraits - buggy version
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-Range NT AsRange",
                                        CopyImageRegionRangeNumericTraitsAsRange<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);
}


int
main(int argc, char * argv[])
{
  if (argc < 4)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " timingsFile iterations imageSize" << std::endl;
    return EXIT_FAILURE;
  }
  const std::string timingsFileName = ReplaceOccurrence(argv[1], "__DATESTAMP__", PerfDateStamp());
  const int         iterations = std::stoi(argv[2]);
  const int         imageSize = std::stoi(argv[3]);

  constexpr unsigned int Dimension = 3;
  const auto             size = itk::Size<Dimension>::Filled(imageSize);

  std::ostringstream oss;
  oss << "Image Size: " << size;
  std::string description = oss.str();

  itk::HighPriorityRealTimeProbesCollector collector;

  // Test 1: Image<Vector> to Image<RGBPixel>
  TimeIterationMethods<itk::Image<itk::Vector<float, 3>, Dimension>, itk::Image<itk::RGBPixel<double>, Dimension>>(
    collector, size, "IVf3->IRGB", iterations);

  // Test 2: VectorImage to Image<Vector>
  TimeIterationMethods<itk::VectorImage<float, Dimension>, itk::Image<itk::Vector<double, 3>, Dimension>>(
    collector, size, "VIf->IVd3", iterations);

  // Test 3: Image<Vector> to VectorImage
  TimeIterationMethods<itk::Image<itk::Vector<float, 3>, Dimension>, itk::VectorImage<double, Dimension>>(
    collector, size, "IVf3->VId", iterations);

  // Test 4: VectorImage to VectorImage
  TimeIterationMethods<itk::VectorImage<float, Dimension>, itk::VectorImage<double, Dimension>>(
    collector, size, "VIf->VId", iterations);

  WriteExpandedReport(timingsFileName, collector, true, true, false);


  return EXIT_SUCCESS;
}
