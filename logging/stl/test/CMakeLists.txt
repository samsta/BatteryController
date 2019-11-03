include(GoogleTest)

add_executable(test_logging_stl test_Hex.cpp)
target_link_libraries(test_logging_stl logging gmock gmock_main)
target_include_directories(test_logging_stl PUBLIC ../)
add_test(test_logging_stl test_logging_stl)