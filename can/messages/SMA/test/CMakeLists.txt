include(GoogleTest)

add_executable(test_can_messages_sma test_BatteryEvents.cpp
                                     test_BatteryIdentity.cpp
                                     test_BatteryMeasurements.cpp
                                     test_BatteryLimits.cpp
                                     test_BatteryState.cpp
                                     test_BatterySystemInfo.cpp)

target_link_libraries(test_can_messages_sma can gmock gmock_main)
add_test(test_can_messages_sma test_can_messages_sma)