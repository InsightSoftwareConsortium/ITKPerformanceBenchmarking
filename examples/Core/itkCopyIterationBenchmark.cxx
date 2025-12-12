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

// This Benchmark compares the performance of ImageRegionIterator, ImageScanlineIterator,
// and ImageRegionRange for simple pixel copying with static_cast operations.

#include <iostream>
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkFixedArray.h"
#include "itkImageRegionRange.h"
#include "itkImageScanlineIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageAlgorithm.h"
#include "itkHighPriorityRealTimeProbesCollector.h"
#include "PerformanceBenchmarkingUtilities.h"
#include <iomanip>
#include <fstream>


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

  itk::ImageRegionIterator<TImage> it(image, region);
  for (; !it.IsAtEnd(); ++it)
  {
    it.Set(static_cast<PixelType>(count));
    ++count;
  }

  return image;
}


// Method 0: ImageAlgorithm::Copy
template <typename TInputImage, typename TOutputImage>
void
CopyImageAlgorithm(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  itk::ImageAlgorithm::Copy(inputPtr, outputPtr, inputPtr->GetBufferedRegion(), outputPtr->GetBufferedRegion());
}

// Method 1: ImageRegionIterator approach
template <typename TInputImage, typename TOutputImage>
void
CopyRegionIterator(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;
  using ImageRegionConstIterator = itk::ImageRegionConstIterator<TInputImage>;
  using ImageRegionIterator = itk::ImageRegionIterator<TOutputImage>;

  const typename TOutputImage::RegionType outputRegion = outputPtr->GetRequestedRegion();
  typename TInputImage::RegionType        inputRegion = outputRegion;

  ImageRegionConstIterator inputIt(inputPtr, inputRegion);
  ImageRegionIterator      outputIt(outputPtr, outputRegion);

  for (; !inputIt.IsAtEnd(); ++inputIt, ++outputIt)
  {
    outputIt.Set(static_cast<OutputPixelType>(inputIt.Get()));
  }
}


// Method 2: ImageScanlineIterator approach
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

  while (!inputIt.IsAtEnd())
  {
    while (!inputIt.IsAtEndOfLine())
    {
      outputIt.Set(static_cast<OutputPixelType>(inputIt.Get()));
      ++inputIt;
      ++outputIt;
    }
    inputIt.NextLine();
    outputIt.NextLine();
  }
}


// Method 3: ImageRegionRange approach
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

  while (inputIt != inputEnd)
  {
    *outputIt = OutputPixelType(InputPixelType(*inputIt));
    ++inputIt;
    ++outputIt;
  }
}


// Method 4: ImageRegionRange with range-based for loop
template <typename TInputImage, typename TOutputImage>
void
CopyImageRegionRangeForLoop(const TInputImage * inputPtr, TOutputImage * outputPtr)
{
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;

  const typename TOutputImage::RegionType outputRegion = outputPtr->GetRequestedRegion();
  typename TInputImage::RegionType        inputRegion = outputRegion;

  auto outputRange = itk::ImageRegionRange<TOutputImage>(*outputPtr, outputRegion);
  auto outputIt = outputRange.begin();

  for (const InputPixelType & inputPixel : itk::ImageRegionRange<const TInputImage>(*inputPtr, inputRegion))
  {
    *outputIt = static_cast<OutputPixelType>(inputPixel);
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
  if (inputImage->GetNumberOfComponentsPerPixel() > 0)
  {
    outputImage->SetNumberOfComponentsPerPixel(inputImage->GetNumberOfComponentsPerPixel());
  }
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
                     int                                        iterations,
                     unsigned int                               numberOfComponentsPerPixel = 0)
{
  // Create and initialize input image
  auto inputImage = CreateAndInitializeImage<TInputImage>(size, numberOfComponentsPerPixel);

  typename TOutputImage::Pointer outputImage;

  // Test Method 0: ImageAlgorithm::Copy
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-ImageAlgorithm",
                                        CopyImageAlgorithm<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 1: Region Iterator
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-RegionIterator",
                                        CopyRegionIterator<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 2: Scanline Iterator
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-ScanlineIterator",
                                        CopyScanlineIterator<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 3: ImageRegionRange
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-Range",
                                        CopyImageRegionRange<TInputImage, TOutputImage>,
                                        inputImage.GetPointer(),
                                        outputImage,
                                        iterations);

  // Test Method 4: ImageRegionRange with range-based for loop
  TimeMethod<TInputImage, TOutputImage>(collector,
                                        description + "-RangeForLoop",
                                        CopyImageRegionRangeForLoop<TInputImage, TOutputImage>,
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

  itk::HighPriorityRealTimeProbesCollector collector;

  // Test 1: uint16 to int16
  TimeIterationMethods<itk::Image<unsigned short, Dimension>, itk::Image<short, Dimension>>(
    collector, size, "Iu2->Ii2", iterations);

  // Test 2: FixedArray<float,3> to FixedArray<double,3>
  TimeIterationMethods<itk::Image<itk::FixedArray<float, 3>, Dimension>,
                       itk::Image<itk::FixedArray<double, 3>, Dimension>>(collector, size, "IFf3->IFd3", iterations);

  // Test 3: VectorImage<float> to VectorImage<double> with 3 components
  TimeIterationMethods<itk::VectorImage<float, Dimension>, itk::VectorImage<double, Dimension>>(
    collector, size, "IVf->IVd", iterations, 3);

  WriteExpandedReport(timingsFileName, collector, true, true, false);

  return EXIT_SUCCESS;
}
