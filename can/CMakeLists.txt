cmake_minimum_required(VERSION 3.5)

add_library(can DataFrame.cpp)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()