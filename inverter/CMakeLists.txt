add_library(inverter)
target_include_directories(inverter PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(SMA)
add_subdirectory(TSUN)
add_subdirectory(SINEX)
