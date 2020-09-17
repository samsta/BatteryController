include(GoogleTest)

add_executable(test_can_messages_sma test_BatteryEvents.cpp
                                     test_BatteryState.cpp)

target_link_libraries(test_can_messages_sma can gmock gmock_main)
add_test(test_can_messages_sma test_can_messages_sma)