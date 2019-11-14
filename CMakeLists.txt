cmake_minimum_required(VERSION 3.13)
project(BatteryController)
cmake_policy(SET CMP0076 NEW)

set(HAS_STD_IOSTREAM TRUE CACHE BOOL "std::iostream and friends are available")
set(ARDUINO_BOARD "diecimila" CACHE STRING "Arduino board to build for")
set(ARDUINO_CPU "atmega328" CACHE STRING "Arduino CPU on ARDUINO_BOARD")
set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_SOURCE_DIR}/valgrind.supp")

set(CMAKE_CXX_FLAGS  "-Wall -Werror -Wextra -g -std=c++0x")

include(CTest)

if(CMAKE_SYSTEM_NAME MATCHES "Arduino")
  set(BUILD_TESTING FALSE)
  set(HAS_STD_IOSTREAM FALSE)
  get_board_id(board_id ${ARDUINO_BOARD} ${ARDUINO_CPU})
endif()

configure_file(config.h.in config.h)

if(BUILD_TESTING)
  add_subdirectory(googletest)
  add_custom_target(memcheck
    COMMAND ${CMAKE_CTEST_COMMAND}
        --output-on-failure --force-new-ctest-process --test-action memcheck -DMEMORYCHECK_SUPPRESSIONS_FILE="${CMAKE_SOURCE_DIR}/valgrind.supp"
    COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log")
else()
  message(STATUS "NOT building tests")
endif()

add_subdirectory(can)
add_subdirectory(logging)
add_subdirectory(monitor)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
   # horrible test tool that receives messages

   find_file(GPIOD_HEADER gpiod.h)
   if (GPIOD_HEADER MATCHES "GPIOD_HEADER-NOTFOUND")
      message(STATUS "Won't build testcan tool because I couldn't find gpiod.h")
      return()
   endif()

   find_library(GPIOD_LIBRARY gpiod)
   if (GPIOD_LIBRARY MATCHES "GPIOD_LIBRARY-NOTFOUND")
      message(STATUS "Won't build testcan tool because I couldn't find libgpiod")
      return()
   endif()

   add_executable(testcan testcan.cpp)
   target_link_libraries(testcan can logging monitor ${GPIOD_LIBRARY})
endif()