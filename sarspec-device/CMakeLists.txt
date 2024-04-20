cmake_minimum_required (VERSION 3.10)

project (sarspec-lib)

set( CMAKE_CXX_FLAGS "-fPIC" )

include_directories( ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

add_library(sarspec SHARED sarspec-device.h sarspec-device.cpp)
add_executable(sarspec-test sarspec-device-test.cpp )

target_link_libraries(sarspec ftdi1)
target_link_libraries(sarspec-test sarspec)

SET(CPACK_GENERATOR "TGZ;DEB")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SarspecRes+ User API")
SET(CPACK_PACKAGE_VENDOR "Instituto de Plasmas e Fusão Nuclear / IST")
SET(CPACK_PACKAGE_CONTACT "Vasco Dinis Mendes Freitas <vasco.d.freitas@tecnico.ulisboa.pt>")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

INCLUDE(CPack)

#install( TARGETS sarspec-device-api sarspec-device RUNTIME DESTINATION bin ARCHIVE DESTINATION lib )
#install( FILES sarspec-device.h DESTINATION include )


