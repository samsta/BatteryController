include(GoogleTest)

add_executable(test_can_services_nissan test_FrameAggregator.cpp
                                        test_GroupPoller.cpp)
target_link_libraries(test_can_services_nissan can gmock gmock_main)
gtest_discover_tests(test_can_services_nissan)