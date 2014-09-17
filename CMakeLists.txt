cmake_minimum_required (VERSION 2.6)
project (Drifter)

set( DRIFTER_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/install_test" CACHE STRING "Install location." )
set( TRACKING_INSTALL_DIR "${DRIFTER_INSTALL_DIR}" CACHE STRING "Tracking installation directory." )
set( DRIFTERCAR_INSTALL_DIR "${DRIFTER_INSTALL_DIR}" CACHE STRING "Drifter car installation director." )
set( TRACKING_INSTALL_DIR "${DRIFTER_INSTALL_DIR}" )
set( DRIFTERCAR_INSTALL_DIR "${DRIFTER_INSTALL_DIR}" )

add_subdirectory( objectTracking )

add_subdirectory( drifterCar )
