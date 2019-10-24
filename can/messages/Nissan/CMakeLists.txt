target_sources(can PRIVATE CellVoltages.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()