cmake_minimum_required (VERSION 3.5)


# enable_language(Fortran)

project(SpectraSolver)
# get_filename_component(Fortran_COMPILER_NAME ${CMAKE_Fortran_COMPILER} NAME)
# 
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
# SET(CMAKE_CXX_FLAGS "-march=native")
# set(CMAKE_CXX_FLAGS "-Ofast")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/FindLAPACKE.cmake")

# Locate  FFTW3
find_package(FFTW)

# Try to find Eigen3 on the system first (without making it REQUIRED)
find_package(Eigen3 3.3 QUIET)

# If it wasn't found, download it automatically
if(NOT Eigen3_FOUND)
    message(STATUS "Eigen3 not found locally, downloading via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
      Eigen3
      GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
      GIT_TAG 3.4.0   # Note: I changed 'master' to a stable release tag (3.4.0) to prevent future breakage
    )
    FetchContent_MakeAvailable(Eigen3)
endif()

# find_package (FFTWpp)
include(FetchContent)
FetchContent_Declare(
  FFTWpp
  GIT_REPOSITORY https://github.com/da380/FFTWpp
  GIT_TAG main
)
FetchContent_MakeAvailable(FFTWpp)

find_package(OpenMP REQUIRED)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# find_package (LAPACK)
# Set up the library
add_library (SpectraSolver INTERFACE)
target_link_libraries(SpectraSolver INTERFACE Eigen3::Eigen)
target_link_libraries(SpectraSolver INTERFACE FFTWpp)
target_link_libraries(SpectraSolver INTERFACE OpenMP::OpenMP_CXX)
target_include_directories (SpectraSolver INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)

# optionally add in the examples and tests
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # We're in the root, define additional targets for developers.
    option(MY_PROJECT_BUILD_EXAMPLES   "whether or not examples should be built" ON)

    if(MY_PROJECT_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

# blas and lapack

# find_package(LAPACKE REQUIRED)
# add_subdirectory(LAPACKE)
