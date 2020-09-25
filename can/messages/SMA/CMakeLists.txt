target_sources(can PRIVATE BatteryLimits.cpp
                           BatteryState.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()