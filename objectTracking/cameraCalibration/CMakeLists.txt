add_executable( cameraCalibration camera_calibration.cpp )

target_link_libraries( cameraCalibration objectTracking )
target_link_libraries( cameraCalibration ${OpenCV_LIBS} )
target_link_libraries( cameraCalibration ${Boost_LIBRARIES} )

install ( TARGETS cameraCalibration DESTINATION ${TRACKING_INSTALL_DIR}/bin )
install ( FILES default_calibration_settings.xml DESTINATION ${TRACKING_INSTALL_DIR}/bin )
install ( FILES OpenCV_Chessboard.png DESTINATION ${TRACKING_DATA_DIR}/camera/ )