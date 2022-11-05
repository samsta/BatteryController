/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _PACKS_NISSAN_LEAFPACK_HPP_
#define _PACKS_NISSAN_LEAFPACK_HPP_

#include "monitor/Nissan/LeafMonitor.hpp"
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/services/Nissan/MessageFactory.hpp"
#include "contactor/Nissan/LeafContactor.hpp"
#include "can/services/Nissan/HappyPoller.hpp"

namespace packs {
namespace Nissan {

class LeafPack: public can::FrameSink
{
public:
   LeafPack(
         can::FrameSink& sender,
         core::Timer& timer,
         logging::ostream* log);
   
   // ~LeafPack();

   virtual void sink(const can::DataFrame& f);

   monitor::Monitor& getMonitor();
   contactor::Contactor& getContactor();

private:

   contactor::Nissan::TeensyRelay         m_safety_contactor;
   monitor::Nissan::LeafMonitor           m_monitor;
   core::Timer&                           m_timer;
   can::services::Nissan::MessageFactory  m_message_factory;
   can::services::Nissan::FrameAggregator m_aggregator;
   can::services::Nissan::GroupPoller     m_poller;
   can::services::Nissan::HappyPoller     m_happy_poller;
   core::Callback<LeafPack>               m_periodic_callback;

   void periodicCallback();

};

}
}

#endif /* _PACKS_NISSAN_LEAFPACK_HPP_ */
