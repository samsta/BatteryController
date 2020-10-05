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
   ~SunnyBoyStorage();
private:
   void sendBatteryData();
   
   can::FrameSink&   m_sender;
   core::Timer&      m_timer;
   monitor::Monitor& m_monitor;
   core::Callback<SunnyBoyStorage> m_periodic_callback;
};

}
}

#endif // _INVERTER_SMA_SBS_HPP
