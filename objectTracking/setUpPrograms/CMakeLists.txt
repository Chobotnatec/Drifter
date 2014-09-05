
add_executable( setUpMarker setUpMarker.cpp )

target_link_libraries( setUpMarker ${OpenCV_LIBS} )
target_link_libraries( setUpMarker objectTracking )
target_link_libraries( setUpMarker ${Boost_LIBRARIES} )

add_executable( checkMarkers checkMarkers.cpp )

target_link_libraries( checkMarkers ${OpenCV_LIBS} )
target_link_libraries( checkMarkers objectTracking )
target_link_libraries( checkMarkers ${Boost_LIBRARIES} )

add_executable( showGroundMarkers showGroundMarkers.cpp )

target_link_libraries( showGroundMarkers ${OpenCV_LIBS} )
target_link_libraries( showGroundMarkers objectTracking )
target_link_libraries( showGroundMarkers ${Boost_LIBRARIES} )

add_executable( determineCameraPosition determineCameraPosition.cpp )

target_link_libraries( determineCameraPosition ${OpenCV_LIBS} )
target_link_libraries( determineCameraPosition objectTracking )
target_link_libraries( determineCameraPosition ${Boost_LIBRARIES} )

install ( TARGETS setUpMarker checkMarkers showGroundMarkers  DESTINATION ${TRACKING_INSTALL_DIR}/bin )

