
add_library( objectTracking objectTracking.cpp carTracker.cpp )

add_subdirectory( ROITracker )

target_link_libraries( objectTracking nlopt )
target_link_libraries( objectTracking ${OpenCV_LIBS} )
target_link_libraries( objectTracking ROITracker )

install ( TARGETS objectTracking DESTINATION ${DRIFTER_INSTALL_DIR}/lib )
install ( FILES objectTracking.h carTracker.h DESTINATION ${DRIFTER_INSTALL_DIR}/include )