include(GoogleTest)
find_package(CURL REQUIRED)

add_executable(test_contactor_nissan test_LeafContactor.cpp)
target_link_libraries(test_contactor_nissan contactor can CURL::libcurl gmock gmock_main)
add_test(test_contactor_nissan test_contactor_nissan)