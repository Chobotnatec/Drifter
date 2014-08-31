
add_library( objectTracking objectTracking.cpp carTracker.cpp configParser.cpp )

add_subdirectory( ROITracker )

target_link_libraries( objectTracking nlopt )
target_link_libraries( objectTracking ${OpenCV_LIBS} )
target_link_libraries( objectTracking ROITracker )
target_link_libraries( objectTracking ${Boost_LIBRARIES} )

install ( TARGETS objectTracking DESTINATION ${TRACKING_INSTALL_DIR}/lib )
install ( FILES objectTracking.h carTracker.h DESTINATION ${TRACKING_INSTALL_DIR}/include )