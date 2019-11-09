target_sources(monitor PRIVATE Monitor.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()