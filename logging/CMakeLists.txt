find_package(CURL REQUIRED)
add_library(logging colors.cpp logging.cpp AvgMinMax.cpp)
target_link_libraries(logging CURL::libcurl)
target_include_directories(logging PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(no_stl)
add_subdirectory(stl)

