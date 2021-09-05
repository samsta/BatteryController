target_sources(can PRIVATE InformationRequest.cpp
                           SleepAwakeCommand.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
