
add_library( ROITracker STATIC ROITracker.cpp ProjectUtilities.cpp )

install( FILES ROITracker.h ProjectUtilities.h DESTINATION ${TRACKING_INSTALL_DIR}/include/ROITracker )

install( TARGETS ROITracker DESTINATION ${TRACKING_INSTALL_DIR}/lib )