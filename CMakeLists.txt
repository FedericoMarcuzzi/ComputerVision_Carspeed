
cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(CvLab)

if($ENV{BUILD_NUMBER})
  set(BUILD_NUMBER $ENV{BUILD_NUMBER})
else()
  set(BUILD_NUMBER 0)
endif()

set(CvLab_NAME "car_speed")
set(CvLab_VERSION "0.1.0.${BUILD_NUMBER}")
set(CvLab_BRIEF "${CV2018} is the third assignment of computer vision course 2018")


# Setup default installation dir
#
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
get_filename_component( INSTALL_DIR_ABS "${PROJECT_SOURCE_DIR}/dist" ABSOLUTE)
SET( CMAKE_INSTALL_PREFIX ${INSTALL_DIR_ABS} CACHE PATH "Install prefix" FORCE )
ENDIF()

FIND_PACKAGE( OpenCV REQUIRED )
IF( OpenCV_FOUND )
    MESSAGE( STATUS "OpenCV Version ${OpenCV_VERSION} found.")
ENDIF()

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
add_subdirectory(src)

