add_library(core OutputPin.cpp InputPin.cpp)
target_include_directories(core PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(LibGpiod)
add_subdirectory(SocketCan)
add_subdirectory(USBInterface)
add_subdirectory(WebServer)
add_subdirectory(ModbusServer)
add_subdirectory(${CMAKE_SYSTEM_NAME})