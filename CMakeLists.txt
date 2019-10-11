cmake_minimum_required(VERSION 3.8.2)
project(BatteryController)

set(BUILD_TESTS TRUE CACHE BOOL "Build tests if true")

set(ARDUINO_BOARD "diecimila" CACHE STRING "Arduino board to build for")
set(ARDUINO_CPU "atmega328" CACHE STRING "Arduino CPU on ARDUINO_BOARD")


set(CMAKE_CXX_FLAGS  "-Wall -Werror -Wextra -g -std=c++0x")

if(CMAKE_SYSTEM_NAME MATCHES "Arduino")
  set(BUILD_TESTS FALSE)
  get_board_id(board_id ${ARDUINO_BOARD} ${ARDUINO_CPU})
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(googletest)
else()
  message(STATUS "NOT building tests")
endif()

add_subdirectory(bms)
add_subdirectory(can)
