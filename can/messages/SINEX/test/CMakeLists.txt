include(GoogleTest)

add_executable(test_can_messages_sinex test_InverterHeartbeat.cpp
                                      test_BatteryLimitsOne.cpp
                                      test_BatteryLimitsTwo.cpp
                                      test_BatteryStatus.cpp)

target_link_libraries(test_can_messages_sinex can gmock gmock_main)
add_test(test_can_messages_sinex test_can_messages_sinex)
