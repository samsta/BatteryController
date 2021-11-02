add_library(contactor Contactor.cpp)
target_include_directories(contactor PUBLIC ../ ${CMAKE_BINARY_DIR})

add_subdirectory(Nissan)
