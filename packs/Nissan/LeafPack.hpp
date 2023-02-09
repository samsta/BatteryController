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
         can::FrameSink& sender,
         core::Timer& timer,
         CPlusPlusLogging::Logger* log);
   
   ~LeafPack();

   virtual void sink(const can::DataFrame& f);

   monitor::Monitor& getMonitor();
   contactor::Contactor& getContactor();

private:

   contactor::Nissan::TeensyShuntCtrl     m_safety_shunt;
   monitor::Nissan::LeafMonitor           m_monitor;
   core::Timer&                           m_timer;
   can::services::Nissan::MessageFactory  m_message_factory;
   can::services::Nissan::FrameAggregator m_aggregator;
   can::services::Nissan::GroupPoller     m_poller;
   can::services::Nissan::HappyPoller     m_happy_poller;
   core::Callback<LeafPack>               m_heartbeat_callback;
   unsigned                               m_pack_silent_counter;

   void heartbeatCallback();
   const uint PACK_CALLBACK_PERIOD_ms = 5000;
   const unsigned PACK_SILENT_TIMEOUT_PERIODS = 3;

};

}
}

#endif /* _PACKS_NISSAN_LEAFPACK_HPP_ */
