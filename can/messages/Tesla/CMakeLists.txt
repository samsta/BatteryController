target_sources(can PRIVATE DetailedCellData.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()