/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafPack.hpp>

namespace packs {
namespace Nissan {

LeafPack::LeafPack(
            can::FrameSink& sender,
            core::Timer& timer,
            logging::ostream* log):
   m_safety_contactor(sender, ID_TNSY_DC_SAFE_RLY),
   m_monitor(m_safety_contactor),
   m_timer(timer),
   m_message_factory(m_monitor, log),
   m_aggregator(m_message_factory),
   m_poller(sender, timer),
   m_happy_poller(sender, timer),
   m_periodic_callback(*this, &LeafPack::periodicCallback)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, 5000);
}

// LeafPack::~LeafPack()
// {
//    m_timer.deregisterCallback(&m_periodic_callback);
// }

void LeafPack::periodicCallback()
{
   // monitor the heartbeat, if it goes dead, trigger the safety shunt

}
monitor::Monitor& LeafPack::getMonitor()
{
   return m_monitor;
}

contactor::Contactor& LeafPack::getContactor()
{
   return m_safety_contactor;
}

void LeafPack::sink(const can::DataFrame& f)
{
   m_happy_poller.received(f);
   m_poller.received(f);
   m_aggregator.sink(f);

}


}
}
