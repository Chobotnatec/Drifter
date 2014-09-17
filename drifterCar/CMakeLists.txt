cmake_minimum_required( VERSION 2.8.0.0)

project(DrifterCar)

set( DRIFTERCAR_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../install"  CACHE STRING "Instal location.")

find_package(Boost 1.36.0 COMPONENTS system thread REQUIRED)

message("PRIDAT MULTIPLATFORMNI INICIALIZACI SFML")

if(UNIX)
  list( APPEND CMAKE_CXX_FLAGS "-std=c++11 -fpermissive")
endif(UNIX) 

add_subdirectory( src )

add_subdirectory( test )
