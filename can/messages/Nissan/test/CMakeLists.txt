include(GoogleTest)

add_executable(test_can_messages_nissan test_CellVoltageRange.cpp
                                        test_CellVoltages.cpp
                                        test_BatteryState.cpp
                                        test_BatteryStatus.cpp
                                        test_PackTemperatures.cpp)
target_link_libraries(test_can_messages_nissan can gmock gmock_main)
gtest_discover_tests(test_can_messages_nissan)