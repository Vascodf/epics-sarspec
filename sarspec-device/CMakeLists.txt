cmake_minimum_required (VERSION 3.10)

project (sarspec-device-api)

set( CMAKE_CXX_FLAGS "-fPIC" )

include_directories( ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

add_library( sarspec-device-api SHARED sarspec-device.h sarspec-device.cpp)
add_executable( sarspec-device sarspec-device-test.cpp )

target_link_libraries( sarspec-device-api ftdi1)
target_link_libraries( sarspec-device sarspec-device-api)

SET(CPACK_GENERATOR "TGZ;DEB")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ATCA MIMO32 V2 User API")
SET(CPACK_PACKAGE_VENDOR "Max-Planck-Institut fuer Plasmaphysik")
SET(CPACK_PACKAGE_CONTACT "Torsten Bluhm <torsten.bluhm@ipp.mpg.de>")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

INCLUDE(CPack)

install( TARGETS sarspec-device-api sarspec-device RUNTIME DESTINATION bin ARCHIVE DESTINATION lib )
#install( FILES sarspec-device.h DESTINATION include )


