target_sources(can PRIVATE CellVoltageRange.cpp
                           CellVoltages.cpp
                           BatteryState.cpp
                           Message.cpp
                           PackTemperatures.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()