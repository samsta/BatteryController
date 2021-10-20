add_library(contactor)
target_include_directories(contactor PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(Nissan)
