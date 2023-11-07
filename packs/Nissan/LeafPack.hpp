/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _PACKS_NISSAN_LEAFPACK_HPP_
#define _PACKS_NISSAN_LEAFPACK_HPP_

#include "monitor/Nissan/LeafMonitor.hpp"
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/services/Nissan/MessageFactory.hpp"
#include "contactor/Nissan/LeafContactor.hpp"
#include "can/services/Nissan/HappyPoller.hpp"
#include "logging/logging.hpp"

namespace packs {
namespace Nissan {

class LeafPack: public can::FrameSink
{
public:
   LeafPack(
         char* packname,
         can::FrameSink& sender,
         core::Timer& timer,
         logging::Logger* log);
   
   ~LeafPack();

   virtual void sink(const can::DataFrame& f);

   monitor::Monitor& getMonitor();
   contactor::Contactor& getContactor();
   char* getPackName();

private:

   char                                   *m_pack_name;
   contactor::Nissan::TeensyShuntCtrl     m_safety_shunt;
   contactor::Nissan::TeensyRelay         m_power_relay;
   monitor::Nissan::LeafMonitor           m_monitor;
   core::Timer&                           m_timer;
   can::services::Nissan::MessageFactory  m_message_factory;
   can::services::Nissan::FrameAggregator m_aggregator;
   can::services::Nissan::GroupPoller     m_poller;
   can::services::Nissan::HappyPoller     m_happy_poller;
   core::Callback<LeafPack>               m_heartbeat_callback;
   unsigned                               m_pack_silent_counter;
   unsigned                               m_startup_counter;
   logging::Logger                        *m_log;

   bool m_reboot_in_process;
   unsigned m_reboot_wait_count;

   void heartbeatCallback();
   const uint PACK_CALLBACK_PERIOD_ms = 1000;
   const uint PACK_SILENT_TIMEOUT_PERIODS = 60 * 1000 / PACK_CALLBACK_PERIOD_ms;
   const uint REBOOT_WAIT_PERIODS = 5 * 60 * 1000 / PACK_CALLBACK_PERIOD_ms; 
   const uint REBOOT_POWERDOWN_PERIODS = 10 * 1000 / PACK_CALLBACK_PERIOD_ms; 
   const float MAX_SHUNT_OPEN_CURRENT = 1.01;
   const uint MAX_PACK_STARTUP_COUNT = 60 * 1000 / PACK_CALLBACK_PERIOD_ms;
};

}
}

#endif /* _PACKS_NISSAN_LEAFPACK_HPP_ */
