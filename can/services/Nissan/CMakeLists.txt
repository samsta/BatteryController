target_sources(can PRIVATE FrameAggregator.cpp
                           GroupPoller.cpp
                           MessageFactory.cpp
                           HappyPoller.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()