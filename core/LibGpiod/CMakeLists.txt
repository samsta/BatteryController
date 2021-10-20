find_package(LibGpiod)

if(NOT LibGpiod_FOUND)
   return()
endif()

target_sources(core PRIVATE OutputPin.cpp)
