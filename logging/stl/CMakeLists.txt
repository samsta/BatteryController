if(HAS_STD_IOSTREAM)
  target_sources(logging PRIVATE Hex.cpp)
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()