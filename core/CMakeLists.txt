add_library(core OutputPin.cpp)
target_include_directories(core PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(LibGpiod)