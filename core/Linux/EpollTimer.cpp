/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "core/Linux/EpollTimer.hpp"
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>

namespace core {

EpollTimer::EpollTimer(int epoll_fd, const char* name):
   m_epoll_fd(epoll_fd),
   m_fd(timerfd_create(CLOCK_MONOTONIC, 0)),
   m_invokable(nullptr),
   m_name(name)
{
}

void EpollTimer::registerPeriodicCallback(core::Invokable* invokable, unsigned period_ms)
{
   setTimer(invokable, period_ms, PERIODIC);
}

void EpollTimer::schedule(core::Invokable* invokable, unsigned delay_ms)
{
   setTimer(invokable, delay_ms, ONE_SHOT);
}

void EpollTimer::expired()
{
   uint64_t num_expirations = 0;
   if(read(m_fd, &num_expirations, sizeof(num_expirations)) != sizeof(num_expirations))
   {
      std::cerr << "WARNING: unable to determine how many times I misseed timer " << m_name << ": " << strerror(errno) << std::endl;
   }
   if (num_expirations > 1)
   {
      std::cerr << "WARNING: timer " << m_name << " missed " << (num_expirations - 1) << " times" << std::endl;
   }
   m_invokable->invoke();
}

void EpollTimer::setTimer(core::Invokable* invokable, unsigned time_ms, TimerType type)
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

   if (m_invokable == nullptr)
   {
      struct epoll_event ev;
      ev.events = EPOLLIN;
      ev.data.fd = m_fd;
      if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &ev) == -1)
      {
         std::cerr << "ERROR in epoll_ctl(): Failed adding timer " << m_name << " to epoll: " << strerror(errno) << std::endl;
         exit(EXIT_FAILURE);
      }
      m_invokable = invokable;
   }
}

int EpollTimer::fd() const
{
   return m_fd;
}

}
