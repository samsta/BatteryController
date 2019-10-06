cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS  "-Wall -g -std=c++0x")

enable_testing()

add_subdirectory(bms)
add_subdirectory(can)
add_subdirectory(googletest)
