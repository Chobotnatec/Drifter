add_executable( drifter_simple GamePad.cpp )

target_link_libraries( drifter_simple drifterCar sfml-window )

install( TARGETS drifter_simple DESTINATION ${DRIFTERCAR_INSTALL_DIR}/bin )