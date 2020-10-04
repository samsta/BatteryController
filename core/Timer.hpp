/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_TIMER_HPP
#define _CORE_TIMER_HPP

#include "core/Callback.hpp"

namespace core {

class Timer
{
public:
   virtual ~Timer(){}
   virtual void registerPeriodicCallback(Invokable& invokable, unsigned period_ms) = 0;
   virtual void deregisterCallback(Invokable& invokable) = 0;
};

}

#endif // _CORE_TIMER_HPP
