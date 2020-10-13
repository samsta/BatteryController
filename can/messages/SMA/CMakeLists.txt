target_sources(can PRIVATE BatteryIdentity.cpp
                           BatteryLimits.cpp
                           BatteryMeasurements.cpp
                           BatteryState.cpp
                           BatterySystemInfo.cpp
                           InverterCommand.cpp
                           InverterIdentity.cpp
                           String.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()