cmake_minimum_required (VERSION 2.6)
project (ObjectTrackingLibrary)
 
# install directory for objectTracking library
set( DRIFTER_INSTALL_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/.."  CACHE STRING "Instal location!.")

# find external libraries
find_package( Boost 1.3.6 REQUIRED )
find_package( OpenCV REQUIRED )
find_package( Eigen3 REQUIRED )

# set up NLOPT
message( "ADD CROSS-PLATFORM INITIALIZATION OF NLOPT" )

# add include directories
include_directories( ${EIGEN3_INCLUDE_DIR} )

# make sure that c++11 is enabled in unix machine
if(UNIX)
  list( APPEND CMAKE_CXX_FLAGS "-std=c++11")
endif(UNIX) 


# Create library `objectTracking`
add_subdirectory( library )

add_subdirectory( examples )