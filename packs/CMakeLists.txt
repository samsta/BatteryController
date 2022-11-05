add_library(packs)
target_link_libraries(packs can contactor monitor)
target_include_directories(packs PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(Nissan)

