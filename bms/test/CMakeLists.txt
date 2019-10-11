include(GoogleTest)

add_executable(test_bms test_BMS.cpp)
target_link_libraries(test_bms bms can gmock gmock_main)
gtest_add_tests(TARGET test_bms)