target_sources(can PRIVATE FrameAggregator.cpp
                           GroupPoller.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()