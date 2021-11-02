find_package(Curses)

target_sources(core PRIVATE EpollTimer.cpp)

if (CURSES_FOUND)
   target_sources(core PRIVATE ConsolePresenter.cpp)
   target_link_libraries(core ${CURSES_LIBRARIES})
endif()