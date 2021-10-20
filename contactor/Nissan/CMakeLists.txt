target_sources(contactor PRIVATE LeafContactor.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()