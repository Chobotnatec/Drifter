add_library(drifterCar DrifterCar.cpp PacketUtils.cpp TimeoutSerial.cpp)

target_link_libraries( drifterCar ${Boost_LIBRARIES} ) 

install( TARGETS drifterCar DESTINATION ${DRIFTERCAR_INSTALL_DIR}/lib )
install( FILES DrifterCar.h PacketUtils.h TimeoutSerial.h DESTINATION ${DRIFTERCAR_INSTALL_DIR}/include )