cmake_minimum_required(VERSION 3.5)

add_executable(test_BMS test_BMS.cpp)
target_link_libraries(test_BMS bms can gmock gmock_main)
add_test(test_BMS test_BMS)