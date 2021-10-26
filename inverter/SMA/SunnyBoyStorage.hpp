/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _INVERTER_SMA_SBS_HPP
#define _INVERTER_SMA_SBS_HPP

#include "can/FrameSink.hpp"
#include "can/messages/Message.hpp"
#include "contactor/Contactor.hpp"
#include "core/Timer.hpp"
#include "inverter/Inverter.hpp"
#include "monitor/Monitor.hpp"

namespace can {
namespace messages {
namespace SMA {
class InverterCommand;
class InverterIdentity;
}
}
}
namespace inverter {
namespace SMA {

class SunnyBoyStorage: public Inverter, public can::messages::MessageSink
{
public:
   SunnyBoyStorage(can::FrameSink& sender,
                   core::Timer& timer,
                   monitor::Monitor& monitor,
                   contactor::Contactor& contactor);
   ~SunnyBoyStorage();

   virtual void sink(const can::messages::Message&);
   
private:
   void process(const can::messages::SMA::InverterCommand&);
   void process(const can::messages::SMA::InverterIdentity&);
   void sendBatteryData();
   
   can::FrameSink&       m_sender;
   core::Timer&          m_timer;
   monitor::Monitor&     m_monitor;
   contactor::Contactor& m_contactor;
   core::Callback<SunnyBoyStorage> m_periodic_callback;
   unsigned              m_inverter_silent_counter;
};

}
}

#endif // _INVERTER_SMA_SBS_HPP
