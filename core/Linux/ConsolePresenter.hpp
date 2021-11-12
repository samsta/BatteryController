/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_LINUX_CONSOLEPRESENTER_HPP_
#define _CORE_LINUX_CONSOLEPRESENTER_HPP_

#include "core/Timer.hpp"
#include "monitor/Monitor.hpp"
#include "contactor/Contactor.hpp"

namespace core {

class ConsolePresenter
{
public:
   ConsolePresenter(core::Timer& timer);
   ~ConsolePresenter();

   void setMonitor(const monitor::Monitor& monitor);
   void setContactor(const contactor::Contactor& contactor);
  
   bool isOperational() const;
  
private:
   void refreshDisplay();

   core::Timer&                     m_timer;
   const monitor::Monitor*          m_monitor;
   const contactor::Contactor*      m_contactor;
   core::Callback<ConsolePresenter> m_refresh_callback;
   bool                             m_have_curses;
};

}

#endif /* _CORE_LINUX_CONSOLEPRESENTER_HPP_ */
