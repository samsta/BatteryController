include(GoogleTest)

add_executable(test_can_messages_tsun test_InformationRequest.cpp
                                      test_SleepAwakeCommand.cpp
                                      test_ChargeDischargeCommand.cpp)

target_link_libraries(test_can_messages_tsun can gmock gmock_main)
add_test(test_can_messages_tsun test_can_messages_tsun)
