target_sources(can PRIVATE DetailedCellData.cpp
                           Message.cpp
                           TSTemperatures.cpp
                           TSCellVoltages.cpp
                           TSBatteryStatus.cpp
                           TSCurrentEnergy.cpp
                           TSModCellVoltages.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()