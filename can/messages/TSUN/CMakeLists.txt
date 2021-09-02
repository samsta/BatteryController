target_sources(can PRIVATE EnsembleInformation.cpp
						   SystemEquipment.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()