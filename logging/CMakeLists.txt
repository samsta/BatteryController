
add_library(logging no_stl/Hex.cpp)

if(HAS_STD_IOSTREAM)
   target_sources(logging PUBLIC stl/Hex.cpp)
endif()

target_include_directories(logging PUBLIC ../ ${CMAKE_BINARY_DIR})
