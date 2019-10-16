target_sources(can PRIVATE DetailedCellData.cpp)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()