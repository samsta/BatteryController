find_package(USBInterface)

if(NOT USBInterface_FOUND)
   return()
endif()

target_sources(core PRIVATE USBInterface.cpp)