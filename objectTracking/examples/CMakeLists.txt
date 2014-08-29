
add_executable( preprocessImage preprocessImage.cpp )

target_link_libraries( preprocessImage ${OpenCV_LIBS} )
target_link_libraries( preprocessImage objectTracking )
