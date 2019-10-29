target_sources(can PRIVATE CellVoltages.cpp
                           PackTemperatures.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()