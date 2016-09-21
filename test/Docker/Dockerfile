FROM debian:8
MAINTAINER Insight Software Consortium <community@itk.org>

RUN apt-get update && apt-get install -y curl && \
  sed -i "s/httpredir.debian.org/`curl -s -D - http://httpredir.debian.org/demo/debian/ | awk '/^Link:/ { print $2 }' | sed -e 's@<http://\(.*\)/debian/>;@\1@g'`/" /etc/apt/sources.list

RUN apt-get update && apt-get install -y \
  build-essential \
  curl \
  git \
  libexpat1-dev \
  libcurl4-openssl-dev \
  libssl-dev \
  libhdf5-dev \
  libjpeg-dev \
  libpng12-dev \
  libpython3-dev \
  libtiff5-dev \
  python \
  ninja-build \
  wget \
  vim \
  zlib1g-dev

RUN mkdir -p /usr/src/ITKPerformanceBenchmarking-build && \
  mkdir -p /usr/src/ITKBenchmarks-build
WORKDIR /usr/src

RUN git clone git://cmake.org/cmake.git CMake && \
  cd CMake && \
  git checkout v3.5.2 && \
  cd .. && mkdir CMake-build && cd CMake-build && \
  /usr/src/CMake/bootstrap \
    --parallel=$(nproc) \
    --prefix=/usr && \
  make -j$(nproc) && \
  ./bin/cmake -DCMAKE_USE_SYSTEM_CURL:BOOL=ON \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DCMAKE_USE_OPENSSL:BOOL=ON . && \
  make install && \
  cd .. && \
  rm -rf CMake*

# ITK master 2016-05-30
ENV ITK_GIT_TAG 1257b0468c147ba2b1a65bae67169bdf35f4fa50
RUN git clone https://itk.org/ITK.git && \
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
