add_library(can DataFrame.cpp)
target_include_directories(can PUBLIC ../)

add_subdirectory(messages)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()