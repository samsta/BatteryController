target_sources(can PRIVATE BatteryIdentity.cpp
                           BatteryLimits.cpp
                           BatteryMeasurements.cpp
                           BatteryState.cpp
                           BatterySystemInfo.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()