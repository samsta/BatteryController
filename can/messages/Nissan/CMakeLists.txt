target_sources(can PRIVATE CellVoltages.cpp
                           BatteryState.cpp
                           PackTemperatures.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()