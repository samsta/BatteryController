find_package(SocketCan)

if(NOT SocketCan_FOUND)
   return()
endif()

target_sources(core PRIVATE USBInterface.cpp)