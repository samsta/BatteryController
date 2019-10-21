add_library(can DataFrame.cpp
                LargeDataFrame.cpp
                StandardDataFrame.cpp)
target_include_directories(can PUBLIC ../ ${CMAKE_BINARY_DIR})
target_link_libraries(can logging)

add_subdirectory(messages)
add_subdirectory(services)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()