include(GoogleTest)

add_executable(test_can_messages_nissan test_CellVoltages.cpp
                                        test_PackTemperatures.cpp)
target_link_libraries(test_can_messages_nissan can gmock gmock_main)
add_test(test_can_messages_nissan test_can_messages_nissan)