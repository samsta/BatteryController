include(GoogleTest)

add_executable(test_can test_StandardDataFrame.cpp)
target_link_libraries(test_can can gmock gmock_main)
target_include_directories(test_can PUBLIC ../)
gtest_add_tests(TARGET test_can)