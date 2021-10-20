add_library(logging colors.cpp
                    logging.cpp)
target_include_directories(logging PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(no_stl)
add_subdirectory(stl)

