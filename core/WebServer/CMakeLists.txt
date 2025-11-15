find_package(SocketCan)

if(NOT SocketCan_FOUND)
   return()
endif()

add_library(mongoose STATIC mongoose.c)
target_link_libraries(core mongoose pthread)

target_sources(core PRIVATE WebServer.cpp)