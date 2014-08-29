
add_executable( determineCameraPosition determineCameraPosition.cpp ) 
target_link_libraries( determineCameraPosition objectTracking )

add_executable( showGroundMarkers showGroundMarkers.cpp ) 
target_link_libraries( showGroundMarkers objectTracking )

add_executable( trackCar trackCar.cpp )
target_link_libraries( trackCar objectTracking )

add_executable( recordVideoWithoutControl recordVideoWithoutControl.cpp )
target_link_libraries( recordVideoWithoutControl ${OpenCV_LIBS} )

add_executable(  checkMarkers checkMarkers.cpp )
target_link_libraries( checkMarkers objectTracking )

add_executable( setUpMarker setUpMarker.cpp )
target_link_libraries( setUpMarker objectTracking )

# OBSOLETE SHOULD BE REVIEWD AND POSSIBLY REMOVED - USE DRIFTER_DATA_DIR FOR MARKER LOCATIONS copy marker files
file( COPY ${MARKER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

install( TARGETS determineCameraPosition showGroundMarkers trackCar recordVideoWithoutControl checkMarkers setUpMarker DESTINATION ${DRIFTER_INSTALL_DIR}/bin )


