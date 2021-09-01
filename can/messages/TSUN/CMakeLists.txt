target_sources(can PRIVATE EnsembleInformation.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()