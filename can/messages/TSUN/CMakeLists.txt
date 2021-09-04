target_sources(can PRIVATE EnsembleInformation.cpp
						   SystemEquipment.cpp
						   InformationRequest.cpp
						   SleepAwakeCommand.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()