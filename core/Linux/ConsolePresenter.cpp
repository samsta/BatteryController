/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "ConsolePresenter.hpp"
#include <curses.h>
#include <sstream>

namespace core {

ConsolePresenter::ConsolePresenter(
      core::Timer& timer,
      const monitor::Monitor& monitor,
      const contactor::Contactor& contactor):
  m_timer(timer),
  m_monitor(monitor),
  m_contactor(contactor),
  m_refresh_callback(*this, &ConsolePresenter::refreshDisplay)
{
   initscr();      // init ncurses
   m_timer.registerPeriodicCallback(&m_refresh_callback, 1000);
}

ConsolePresenter::~ConsolePresenter()
{
   m_timer.deregisterCallback(&m_refresh_callback);
}

void ConsolePresenter::refreshDisplay()
{
   std::stringstream ss;
   ss << "Monitor:" << std::endl;
   ss << "--------" << std::endl;
   ss << m_monitor << std::endl;

   ss << "Contactor:" << std::endl;
   ss << "--------" << std::endl;
   ss << m_contactor;
   erase();
   wprintw(stdscr, ss.str().c_str());
   refresh();
}


}
