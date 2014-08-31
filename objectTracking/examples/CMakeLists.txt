
add_executable( preprocessImage preprocessImage.cpp )

target_link_libraries( preprocessImage ${OpenCV_LIBS} )
target_link_libraries( preprocessImage objectTracking )
target_link_libraries( preprocessImage ${Boost_LIBRARIES} )

install ( TARGETS preprocessImage DESTINATION ${TRACKING_INSTALL_DIR}/bin )

