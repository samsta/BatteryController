include(GoogleTest)

add_executable(test_can_messages_tsun test_InformationRequest.cpp
                                      test_SleepAwakeCommand.cpp
                                      test_ChargeDischargeCommand.cpp
                                      test_BatteryLimits.cpp
                                      test_BatteryInfo.cpp
                                      test_BatteryStatus.cpp
                                      test_BatteryCellVoltInfo.cpp
                                      test_BatteryCellTempInfo.cpp
                                      test_BatteryModVoltInfo.cpp
                                      test_BatteryModTempInfo.cpp)

target_link_libraries(test_can_messages_tsun can gmock gmock_main)
add_test(test_can_messages_tsun test_can_messages_tsun)
