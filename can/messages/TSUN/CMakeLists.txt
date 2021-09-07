target_sources(can PRIVATE InformationRequest.cpp
                           SleepAwakeCommand.cpp
                           ChargeDischargeCommand.cpp
                           BatteryLimits.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
