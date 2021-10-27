include(GoogleTest)

add_executable(test_can_messages_nissan test_CellVoltageRange.cpp
                                        test_CellVoltages.cpp
                                        test_BatteryState.cpp
                                        test_BatteryStatus.cpp
                                        test_PackTemperatures.cpp
                                        test_BatteryPowerLimits.cpp
                                        test_BatteryVoltsCurrent.cpp)
target_link_libraries(test_can_messages_nissan can gmock gmock_main)
add_test(test_can_messages_nissan test_can_messages_nissan)