target_sources(can PRIVATE messages/Tesla/DetailedCellData.cpp)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()