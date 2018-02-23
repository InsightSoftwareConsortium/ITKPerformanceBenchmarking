FROM debian:9
MAINTAINER Insight Software Consortium <community@itk.org>

RUN REPO=http://cdn-fastly.deb.debian.org && \
  echo "deb $REPO/debian stretch main\ndeb $REPO/debian stretch-updates main\ndeb $REPO/debian-security stretch/updates main" > /etc/apt/sources.list

RUN apt-get update && apt-get install -y \
  build-essential \
  curl \
  git \
  libexpat1-dev \
  libhdf5-dev \
  libjpeg-dev \
  libpng-dev \
  libtiff5-dev \
  python \
  ninja-build \
  wget \
  zlib1g-dev \
  && apt-get clean

RUN wget https://cmake.org/files/v3.11/cmake-3.11.0-rc1-Linux-x86_64.sh && \
  chmod +x cmake-3.11.0-rc1-Linux-x86_64.sh && \
  ./cmake-3.11.0-rc1-Linux-x86_64.sh --prefix=/usr/ --skip-license && \
  rm cmake-3.11.0-rc1-Linux-x86_64.sh

RUN mkdir -p /ITKPerformanceBenchmarking-build && \
  mkdir -p /ITKBenchmarks-build
WORKDIR /

# ITK master 2018-02-22
ENV ITK_GIT_TAG 4b1e6c03f1f9f07f2d97521103d5f438eef95d52
RUN git clone https://github.com/InsightSoftwareConsortium/ITK.git && \
  cd ITK && \
  git checkout ${ITK_GIT_TAG} && \
  cd ../ && \
  mkdir ITK-build && \
  cd ITK-build && \
  cmake \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX:PATH=/usr \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DBUILD_EXAMPLES:BOOL=OFF \
    -DBUILD_TESTING:BOOL=OFF \
    -DBUILD_SHARED_LIBS:BOOL=OFF \
    -DITK_LEGACY_REMOVE:BOOL=ON \
    -DITK_BUILD_DEFAULT_MODULES:BOOL=OFF \
    -DITK_USE_SYSTEM_LIBRARIES:BOOL=ON \
    -DModule_ITKTestKernel:BOOL=ON \
    -DModule_ITKIOImageBase:BOOL=ON \
    -DModule_ITKIOMeta:BOOL=ON \
    -DModule_ITKIONRRD:BOOL=ON \
    -DModule_ITKIOTransformBase:BOOL=ON \
    -DModule_ITKIOTransformHDF5:BOOL=ON \
    -DModule_ITKSmoothing:BOOL=ON \
    -DModule_ITKImageGradient:BOOL=ON \
    -DModule_ITKCurvatureFlow:BOOL=ON \
    -DModule_ITKRegistrationMethodsv4:BOOL=ON \
    -DModule_ITKMetricsv4:BOOL=ON \
    -DModule_ITKPDEDeformableRegistration:BOOL=ON \
    -DModule_ITKConvolution:BOOL=ON \
    -DModule_ITKRegionGrowing:BOOL=ON \
    -DModule_ITKWatersheds:BOOL=ON \
    -DModule_ITKConnectedComponents:BOOL=ON \
    -DModule_ITKAnisotropicDiffusion:BOOL=ON \
    -DModule_ITKFastMarching:BOOL=ON \
    -DModule_ITKLevelSets:BOOL=ON \
  ../ITK && \
  ninja && \
  find . -name '*.o' -delete
