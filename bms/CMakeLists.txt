cmake_minimum_required(VERSION 3.5)

add_library(bms BMS.cpp)
target_include_directories(bms PUBLIC ../ ${CMAKE_BINARY_DIR})

if(BUILD_TESTS)
  add_subdirectory(test)
  enable_testing()
endif()
