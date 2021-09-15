target_sources(can PRIVATE InverterInfoRequest.cpp
                           InverterSleepAwakeCmd.cpp
                           InverterChargeDischargeCmd.cpp
                           BatteryLimits.cpp
                           BatteryInfo.cpp
                           BatteryStatus.cpp
                           BatteryCellVoltInfo.cpp
                           BatteryCellTempInfo.cpp
                           BatteryModVoltInfo.cpp
                           BatteryModTempInfo.cpp
                           BatteryForbidden.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
