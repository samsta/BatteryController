/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_LINUX_EPOLLTIMER_HPP
#define _CORE_LINUX_EPOLLTIMER_HPP

#include "core/Timer.hpp"
#include <string>

namespace core {

class EpollTimer: public core::Timer
{
public:
   EpollTimer(int epoll_fd, const char* name);

   virtual void registerPeriodicCallback(core::Invokable* invokable, unsigned period_ms);
   virtual void schedule(core::Invokable* invokable, unsigned delay_ms);
   virtual void deregisterCallback(core::Invokable*){}

   void expired();

   int fd() const;

private:
   enum TimerType {
      PERIODIC,
      ONE_SHOT
   };

   void setTimer(core::Invokable* invokable, unsigned time_ms, TimerType type);

   int m_epoll_fd;
   int m_fd;
   core::Invokable* m_invokable;
   std::string m_name;
};

}

#endif // _CORE_LINUX_EPOLLTIMER_HPP
