add_library(packs)
target_include_directories(packs PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(Nissan)

