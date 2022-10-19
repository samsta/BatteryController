/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafPack.hpp>

namespace packs {
namespace Nissan {

LeafPack::LeafPack(
            can::FrameSink& sender,
            core::Timer& timer,
            core::OutputPin& positive_relay,
            core::OutputPin& negative_relay,
            core::OutputPin& indicator,
            logging::ostream* log):
   m_contactor(
      timer,
      positive_relay,
      negative_relay,
      indicator,
      log),
   m_monitor(m_contactor),
   m_message_factory(m_monitor, log),
   m_aggregator(m_message_factory),
   m_poller(sender, timer),
   m_happy_poller(sender, timer)
{
}

monitor::Monitor& LeafPack::getMonitor()
{
   return m_monitor;
}

contactor::Contactor& LeafPack::getContactor()
{
   return m_contactor;
}

void LeafPack::sink(const can::DataFrame& f)
{
   // printf("LeafPack::sink\n"); //JFS
   m_happy_poller.received(f);
   m_poller.received(f);
   m_aggregator.sink(f);

}


}
}
