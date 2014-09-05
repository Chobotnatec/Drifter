cmake_minimum_required (VERSION 2.6)
project (ObjectTrackingLibrary)

# install directory for objectTracking library
set( TRACKING_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../install"  CACHE STRING "Instal location.")
set( TRACKING_DATA_DIR "${TRACKING_INSTALL_DIR}/data/trackingDataFolder" CACHE STRING "Object tracking directory." )

# create configuration file
file(WRITE ${TRACKING_DATA_DIR}/tracking.cfg "dataFolder = ${TRACKING_DATA_DIR}")

# find external libraries

find_package( Boost 1.36.0  REQUIRED program_options )
find_package( OpenCV REQUIRED )
find_package( Eigen3 REQUIRED )

# set up NLOPT
message( "ADD CROSS-PLATFORM INITIALIZATION OF NLOPT" )
message( "DEFINE SOME VARIABLE WHICH HOLDS LIBS TO LINK" )

# add include directories
include_directories( ${EIGEN3_INCLUDE_DIR} )

# make sure that c++11 is enabled on unix machine
if(UNIX)
  list( APPEND CMAKE_CXX_FLAGS "-std=c++11")
endif(UNIX) 

# copy configuration file to bin/ folder
install( FILES ${TRACKING_DATA_DIR}/tracking.cfg DESTINATION ${TRACKING_INSTALL_DIR}/bin )

# Create library `objectTracking`
add_subdirectory( library )

add_subdirectory( examples )

add_subdirectory( setUpPrograms )
