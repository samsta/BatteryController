include(GoogleTest)

add_executable(test_can test_StandardDataFrame.cpp
                        test_LargeDataFrame.cpp)
target_link_libraries(test_can can gmock gmock_main)
target_include_directories(test_can PUBLIC ../)
add_test(test_can test_can)