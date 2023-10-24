include(GoogleTest)

add_executable(test_inverter_sinex test_SE-PWS2.cpp)
target_link_libraries(test_inverter_sinex inverter monitor can gmock gmock_main)
add_test(test_inverter_sinex test_inverter_sinex)