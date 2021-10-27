target_sources(can PRIVATE CellVoltageRange.cpp
                           CellVoltages.cpp
                           BatteryState.cpp
                           BatteryStatus.cpp
                           Message.cpp
                           PackTemperatures.cpp
                           BatteryPowerLimits.cpp
                           BatteryVoltsCurrent.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()