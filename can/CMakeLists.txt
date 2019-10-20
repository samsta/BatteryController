add_library(can DataFrame.cpp
                LargeDataFrame.cpp
                StandardDataFrame.cpp)
target_include_directories(can PUBLIC ../ ${CMAKE_BINARY_DIR})
target_link_libraries(can logging)
add_subdirectory(messages)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()