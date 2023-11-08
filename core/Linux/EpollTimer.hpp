/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_LINUX_EPOLLTIMER_HPP
#define _CORE_LINUX_EPOLLTIMER_HPP

#include "core/Timer.hpp"
#include "logging/logging.hpp"
#include <map>

namespace core {

class EpollTimer: public core::Timer
{
public:
   EpollTimer(int epoll_fd );
   ~EpollTimer();

   virtual void registerPeriodicCallback(core::Invokable* invokable, unsigned period_ms);
   virtual void schedule(core::Invokable* invokable, unsigned delay_ms);
   virtual void deregisterCallback(core::Invokable*){}
   void setLogger(logging::Logger* log);

private:

   class TimerEpollEntry;
   std::map<core::Invokable*, TimerEpollEntry*> m_timers;

   int m_epoll_fd;
   logging::Logger* m_log;
};

}

#endif // _CORE_LINUX_EPOLLTIMER_HPP
