target_sources(can PRIVATE EnsembleInformation.cpp
						   SystemEquipment.cpp
						   InformationRequest.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()