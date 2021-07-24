include(GoogleTest)

add_executable(test_can_messages_sma test_BatteryEvents.cpp
                                     test_BatteryIdentity.cpp
                                     test_BatteryMeasurements.cpp
                                     test_BatteryLimits.cpp
                                     test_BatteryState.cpp
                                     test_BatterySystemInfo.cpp
                                     test_InverterCommand.cpp
                                     test_InverterIdentity.cpp
                                     test_Strings.cpp)

target_link_libraries(test_can_messages_sma can gmock gmock_main)
gtest_discover_tests(test_can_messages_sma)
