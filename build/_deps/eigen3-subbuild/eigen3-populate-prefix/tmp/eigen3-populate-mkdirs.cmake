# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/adcm2/space/SpectraSolver/build/_deps/eigen3-src"
  "/home/adcm2/space/SpectraSolver/build/_deps/eigen3-build"
  "/space/adcm2/SpectraSolver/build/_deps/eigen3-subbuild/eigen3-populate-prefix"
  "/space/adcm2/SpectraSolver/build/_deps/eigen3-subbuild/eigen3-populate-prefix/tmp"
  "/space/adcm2/SpectraSolver/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp"
  "/space/adcm2/SpectraSolver/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src"
  "/space/adcm2/SpectraSolver/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/space/adcm2/SpectraSolver/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/space/adcm2/SpectraSolver/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
