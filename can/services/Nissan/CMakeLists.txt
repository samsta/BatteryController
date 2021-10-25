target_sources(can PRIVATE FrameAggregator.cpp
                           GroupPoller.cpp
                           MessageFactory.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()