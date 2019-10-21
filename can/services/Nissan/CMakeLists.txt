target_sources(can PRIVATE FrameAggregator.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()