target_sources(packs PRIVATE LeafPack.cpp)
target_sources(packs PRIVATE LeafMultiPack.cpp)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()