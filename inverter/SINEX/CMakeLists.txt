target_sources(inverter PRIVATE SE-PWS2.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()