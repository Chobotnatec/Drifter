
add_executable( test main.cpp )

# link appropriate libraries
target_link_libraries( test ${OpenCV_LIBS} )
target_link_libraries( test objectTracking )

add_executable( videoCaptureOptionsAndParameters videoCaptureOptionsAndParameters.cpp  )
target_link_libraries( videoCaptureOptionsAndParameters ${OpenCV_LIBS} )

add_executable( carTrackerTest carTrackerTest.cpp )
target_link_libraries( carTrackerTest objectTracking )
target_link_libraries( carTrackerTest ${OpenCV_LIBS} )

# copy marker files
file( COPY ${MARKER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
