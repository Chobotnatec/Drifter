cmake_minimum_required (VERSION 2.6)
project (ObjectTrackingLibrary)
 

set( DRIFTER_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../data CACHE STRING "Location of data folders e.g. markers, data etc..")
set( DRIFTER_INSTALL_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/.."  CACHE STRING "Instal location!.")

include_directories("C:/SFML-2.1/include" 	${Boost_INCLUDE_DIRS}
	C:/nlopt-2.4.2 
	C:/eigen3.21
	)
link_directories(C:/nlopt-2.4.2)
add_definitions( -DDRIFTER_DATA_DIR="${DRIFTER_DATA_DIR}" )

# Find openCV
set(OpenCV_DIR C:/OpenCV/build)
find_package( OpenCV REQUIRED )

# marker files
file( GLOB MARKER_FILES markers/*.txt )

# Create library `objectTracking`
add_subdirectory( library )

add_subdirectory( tests )

add_subdirectory( carTracking )
