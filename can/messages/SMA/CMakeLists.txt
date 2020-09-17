target_sources(can PRIVATE BatteryState.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()