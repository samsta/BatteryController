/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "ConsolePresenter.hpp"
#include <curses.h>
#include <sstream>

namespace core {

ConsolePresenter::ConsolePresenter(core::Timer& timer):
  m_timer(timer),
  m_monitor(nullptr),
  m_contactor(nullptr),
  m_refresh_callback(*this, &ConsolePresenter::refreshDisplay),
  m_have_curses(false)
{
   if (initscr() == stdscr) // init ncurses
   {
      m_timer.registerPeriodicCallback(&m_refresh_callback, 1000);
      m_have_curses = true;
   }
   else
   {
      endwin();
   }
}

ConsolePresenter::~ConsolePresenter()
{
   m_timer.deregisterCallback(&m_refresh_callback);
   endwin();
}

void ConsolePresenter::setMonitor(const monitor::Monitor& monitor)
{
   m_monitor = &monitor;
}
  
void ConsolePresenter::setContactor(const contactor::Contactor& contactor)
{
   m_contactor = &contactor;
}
  
bool ConsolePresenter::isOperational() const
{
   return m_have_curses;
}
  
void ConsolePresenter::refreshDisplay()
{
   std::stringstream ss;
   ss << "Monitor:" << std::endl;
   ss << "--------" << std::endl;
   if (m_monitor)
   {
      ss << *m_monitor << std::endl;
   }
   else
   {
      ss << "not configured" << std::endl;
   }

   ss << "Contactor:" << std::endl;
   ss << "--------" << std::endl;
   if (m_contactor)
   {
      ss << *m_contactor;
   }
   else
   {
      ss << "not configured" << std::endl;
   }
   
   erase();
   wprintw(stdscr, ss.str().c_str());
   refresh();
}


}
