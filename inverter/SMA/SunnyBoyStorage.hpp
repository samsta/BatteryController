/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _INVERTER_SMA_SBS_HPP
#define _INVERTER_SMA_SBS_HPP

#include "inverter/Inverter.hpp"
#include "can/FrameSink.hpp"
#include "core/Timer.hpp"
#include "monitor/Monitor.hpp"

namespace inverter {
namespace SMA {

class SunnyBoyStorage: public Inverter
{
public:
   SunnyBoyStorage(can::FrameSink& sender,
                   core::Timer& timer,
                   monitor::Monitor& monitor);
   
private:
   can::FrameSink&   m_sender;
   core::Timer&      m_timer;
   monitor::Monitor& m_monitor;
};

}
}

#endif // _INVERTER_SMA_SBS_HPP
