target_sources(can PRIVATE InverterHeartbeat.cpp
                           BatteryLimitsOne.cpp
                           BatteryLimitsTwo.cpp
                           BatteryStatus.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
