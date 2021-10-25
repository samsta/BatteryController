cmake_minimum_required(VERSION 3.13)
project(BatteryController)
cmake_policy(SET CMP0076 NEW)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

set(HAS_STD_IOSTREAM TRUE CACHE BOOL "std::iostream and friends are available")
set(HAS_STD_STRING   TRUE CACHE BOOL "std::string is available")
set(HAS_NEW TRUE CACHE BOOL "new and delete are available")
set(ARDUINO_BOARD "diecimila" CACHE STRING "Arduino board to build for")
set(ARDUINO_CPU "atmega328" CACHE STRING "Arduino CPU on ARDUINO_BOARD")
set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_SOURCE_DIR}/valgrind.supp")

find_package(SocketCan)
find_package(LibGpiod)

set(CMAKE_CXX_FLAGS  "-Wall -Werror -Wextra -g -std=c++0x")

include(CTest)

if(CMAKE_SYSTEM_NAME MATCHES "Arduino")
  set(BUILD_TESTING FALSE)
  set(HAS_STD_IOSTREAM FALSE)
  set(HAS_STD_STRING FALSE)
  set(HAS_NEW FALSE)
  get_board_id(board_id ${ARDUINO_BOARD} ${ARDUINO_CPU})
endif()

configure_file(config.h.in config.h)

if(BUILD_TESTING)
  add_subdirectory(googletest)
  add_custom_target(memcheck
    COMMAND ${CMAKE_CTEST_COMMAND}
        --output-on-failure --force-new-ctest-process --test-action memcheck -DMEMORYCHECK_SUPPRESSIONS_FILE="${CMAKE_SOURCE_DIR}/valgrind.supp"
    COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log")
  add_custom_target(check
    COMMAND env GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND} --verbose)
else()
  message(STATUS "NOT building tests")
endif()

add_subdirectory(can)
add_subdirectory(contactor)
add_subdirectory(core)
add_subdirectory(inverter)
add_subdirectory(logging)
add_subdirectory(monitor)


if(SocketCan_FOUND AND LibGpiod_FOUND)
   # horrible prototype
   add_executable(testcan testcan.cpp)
   target_link_libraries(testcan logging monitor inverter can core contactor ${LibGpiod_LIBRARY})
   
   # horrible battery emulator hack
   add_executable(emulator emulator.cpp)
   target_link_libraries(emulator can)

   # horrible battery emulator hack
   add_executable(mitm mitm.cpp)
   target_link_libraries(mitm can)
 endif()

 add_custom_target(vcan_interfaces
   COMMENT "Bringing up vcan0 and vcan1"
   COMMAND sudo modprobe vcan
   COMMAND sudo ip link add dev vcan0 type vcan
   COMMAND sudo ip link set up vcan0
   COMMAND sudo ip link add dev vcan1 type vcan
   COMMAND sudo ip link set up vcan1
   )

