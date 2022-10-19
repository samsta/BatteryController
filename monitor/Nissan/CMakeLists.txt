target_sources(monitor PRIVATE LeafMonitor.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()