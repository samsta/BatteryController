/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_TIMER_HPP
#define _CORE_TIMER_HPP

#include "core/Callback.hpp"
// forward declaration
namespace logging{
   class Logger;
}

namespace core {

class Timer
{
public:
   virtual ~Timer(){}
   virtual void registerPeriodicCallback(Invokable* invokable, unsigned period_ms, const char* timer_name) = 0;
   virtual void schedule(Invokable* invokable, unsigned delay_ms) = 0;
   virtual void deregisterCallback(Invokable* invokable) = 0;
   virtual void setLogger(logging::Logger* log) = 0;
};

}

#endif // _CORE_TIMER_HPP
