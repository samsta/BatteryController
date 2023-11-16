/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "EpollTimer.hpp"
#include "EpollHandler.hpp"
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>

namespace core {

class EpollTimer::TimerEpollEntry: public EpollHandler
{
public:
   enum TimerType
   {
      PERIODIC,
      ONE_SHOT
   };

   TimerEpollEntry(int epoll_fd, core::Invokable* invokable, const char* timer_name, logging::Logger* log):
      m_epoll_fd(epoll_fd),
      m_fd(timerfd_create(CLOCK_MONOTONIC, 0)),
      m_invokable(invokable),
      m_log(log)
   {
      sprintf(m_timer_name,"%.255s",timer_name);
      struct epoll_event ev;
      ev.events = EPOLLIN;
      ev.data.ptr = this;
      if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &ev) == -1)
      {
         std::cerr << "ERROR in epoll_ctl(): Failed adding timer to epoll: " << strerror(errno) << std::endl;
         exit(EXIT_FAILURE);
      }
   }

   ~TimerEpollEntry()
   {
      setTimer(0, ONE_SHOT);
      epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, NULL);
   }

   virtual void handle()
   {
      uint64_t num_expirations = 0;
      if(read(m_fd, &num_expirations, sizeof(num_expirations)) != sizeof(num_expirations))
      {
         std::cerr << "WARNING: unable to determine how many times I missed timer: " << strerror(errno) << std::endl;
         std::ostringstream s;
         s << "WARNING: unable to determine how many times I missed timer: " << m_timer_name << " err:" << strerror(errno);
         if (m_log) m_log->alarm(s); 
      }
      if (num_expirations > 1)
      {
         std::cerr << "WARNING: timer missed " << (num_expirations - 1) << " times" << std::endl;
         std::ostringstream s;
         s << "WARNING: timer: " << m_timer_name << " missed " << (num_expirations - 1) << " times";
         if (m_log) m_log->alarm(s); 
      }
      m_invokable->invoke();
   }

   void setTimer(unsigned time_ms, TimerType type)
   {
      int sec = time_ms / 1000;
      int ms  = time_ms % 1000;
      struct itimerspec its = itimerspec();
      its.it_value.tv_nsec = ms * 1000000;
      its.it_value.tv_sec  = sec;
      if (type == PERIODIC)
      {
         its.it_interval = its.it_value;
      }
      timerfd_settime(m_fd, 0, &its, NULL);
   }

   int m_epoll_fd;
   int m_fd;
   core::Invokable* m_invokable;
   logging::Logger* m_log;
   char m_timer_name[256];
};

EpollTimer::EpollTimer(int epoll_fd):
   m_epoll_fd(epoll_fd),
   m_log(nullptr)
{
}

EpollTimer::~EpollTimer()
{
   for (auto it = m_timers.begin(); it != m_timers.end(); ++it)
   {
      delete it->second;
   }
}

void EpollTimer::registerPeriodicCallback(core::Invokable* invokable, unsigned period_ms, const char* timer_name)
{
   if (m_timers[invokable] == nullptr)
   {
      m_timers[invokable] = new TimerEpollEntry(m_epoll_fd, invokable, timer_name, m_log);
   }
   m_timers[invokable]->setTimer(period_ms, TimerEpollEntry::PERIODIC);
   std::ostringstream ss;
   ss << "Periodic Timer created: " << timer_name << " " << period_ms << "ms";
   if (m_log) m_log->debug(ss);
}

void EpollTimer::schedule(core::Invokable* invokable, unsigned delay_ms, const char* timer_name)
{
   if (m_timers[invokable] == nullptr)
   {
      m_timers[invokable] = new TimerEpollEntry(m_epoll_fd, invokable, timer_name, m_log);
   }
   m_timers[invokable]->setTimer(delay_ms, TimerEpollEntry::ONE_SHOT);
   std::ostringstream ss;
   ss << "Scheduled Timer created: " << timer_name << " " << delay_ms << "ms";
   if (m_log) m_log->debug(ss);
}

void EpollTimer::setLogger(logging::Logger* log)
{
   m_log = log;
}

}
