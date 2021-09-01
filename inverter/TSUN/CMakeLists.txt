target_sources(inverter PRIVATE TSOL-H50K.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()