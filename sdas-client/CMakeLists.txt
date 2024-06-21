cmake_minimum_required (VERSION 3.10)

project (sdas-lib)

set( CMAKE_CXX_FLAGS "-fPIC" )

include_directories(src/sdas src/xmlrpc++0.7/src)

add_library(sdas-client SHARED src/sdas/Data.cpp  src/sdas/DescriptorUID.cpp                  src/sdas/HEvent.cpp      src/sdas/SEvent.cpp
src/sdas/Date.cpp                                 src/sdas/ErrorInfo.cpp                      src/sdas/Parameter.cpp   src/sdas/Time.cpp
src/sdas/Descriptor.cpp                           src/sdas/Event.cpp                          src/sdas/SdasClient.cpp  src/sdas/TimeStamp.cpp
src/xmlrpc++0.7/src/XmlRpcClient.cpp              src/xmlrpc++0.7/src/XmlRpcServer.cpp        src/xmlrpc++0.7/src/XmlRpcSource.cpp
src/xmlrpc++0.7/src/XmlRpcDispatch.cpp            src/xmlrpc++0.7/src/XmlRpcServerMethod.cpp  src/xmlrpc++0.7/src/XmlRpcUtil.cpp
src/xmlrpc++0.7/src/XmlRpcServerConnection.cpp    src/xmlrpc++0.7/src/XmlRpcSocket.cpp        src/xmlrpc++0.7/src/XmlRpcValue.cpp)

SET(CPACK_GENERATOR "TGZ;DEB")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SDAS-client lib")
SET(CPACK_PACKAGE_VENDOR "Instituto de Plasmas e Fusão Nuclear / IST")
SET(CPACK_PACKAGE_CONTACT "Vasco Dinis Mendes Freitas <vasco.d.freitas@tecnico.ulisboa.pt>")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

INCLUDE(CPack)