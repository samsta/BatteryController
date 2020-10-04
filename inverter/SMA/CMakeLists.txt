target_sources(inverter PRIVATE SunnyBoyStorage.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()