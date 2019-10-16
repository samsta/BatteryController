cmake_minimum_required(VERSION 3.13)
project(BatteryController)
cmake_policy(SET CMP0076 NEW)

set(BUILD_TESTS TRUE CACHE BOOL "Build tests if true")
set(HAS_STD_IOSTREAM TRUE CACHE BOOL "std::iostream and friends are available")

set(ARDUINO_BOARD "diecimila" CACHE STRING "Arduino board to build for")
set(ARDUINO_CPU "atmega328" CACHE STRING "Arduino CPU on ARDUINO_BOARD")

set(CMAKE_CXX_FLAGS  "-Wall -Werror -Wextra -g -std=c++0x")

if(CMAKE_SYSTEM_NAME MATCHES "Arduino")
  set(BUILD_TESTS FALSE)
  set(HAS_STD_IOSTREAM FALSE)
  get_board_id(board_id ${ARDUINO_BOARD} ${ARDUINO_CPU})
endif()

configure_file(config.h.in config.h)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(googletest)
else()
  message(STATUS "NOT building tests")
endif()

add_subdirectory(bms)
add_subdirectory(can)
add_subdirectory(logging)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
   # horrible test tool that receives messages
   add_executable(testcan testcan.cpp)
   target_link_libraries(testcan can logging)
endif()