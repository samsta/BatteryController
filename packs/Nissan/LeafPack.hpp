/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _PACKS_NISSAN_LEAFPACK_HPP_
#define _PACKS_NISSAN_LEAFPACK_HPP_

#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/services/Nissan/MessageFactory.hpp"
#include "contactor/Nissan/LeafContactor.hpp"
#include "monitor/Nissan/Monitor.hpp"

namespace packs {
namespace Nissan {

class LeafPack: public can::FrameSink
{
public:
   LeafPack(
         can::FrameSink& sender,
         core::Timer& timer,
         core::OutputPin& positive_relay,
         core::OutputPin& negative_relay,
         core::OutputPin& indicator,
         logging::ostream* log);

   virtual void sink(const can::DataFrame& f);

   monitor::Monitor& getMonitor();
   contactor::Contactor& getContactor();

private:

   contactor::Nissan::LeafContactor       m_contactor;
   monitor::Nissan::Monitor               m_monitor;
   can::services::Nissan::MessageFactory  m_message_factory;
   can::services::Nissan::FrameAggregator m_aggregator;
   can::services::Nissan::GroupPoller     m_poller;

};

}
}

#endif /* _PACKS_NISSAN_LEAFPACK_HPP_ */