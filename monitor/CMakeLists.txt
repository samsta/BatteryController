add_library(monitor monitor.cpp)
target_include_directories(monitor PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(Nissan)

