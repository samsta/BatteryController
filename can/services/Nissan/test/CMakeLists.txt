include(GoogleTest)

add_executable(test_can_services_nissan test_FrameAggregator.cpp
                                        test_GroupPoller.cpp
                                        test_HappyPoller.cpp)
target_link_libraries(test_can_services_nissan can gmock gmock_main)
add_test(test_can_services_nissan test_can_services_nissan)