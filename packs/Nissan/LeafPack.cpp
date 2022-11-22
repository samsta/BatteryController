/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafPack.hpp>

namespace packs {
namespace Nissan {

const unsigned PACK_SILENT_TIMEOUT_PERIODS = 1;

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
   m_heartbeat_callback(*this, &LeafPack::heartbeatCallback),
   m_pack_silent_counter(0)
{
   m_timer.registerPeriodicCallback(&m_heartbeat_callback, 30000);
}

LeafPack::~LeafPack()
{
   m_timer.deregisterCallback(&m_heartbeat_callback);
}

void LeafPack::heartbeatCallback()
{
   // monitor the heartbeat, if it goes dead, trigger the safety shunt
   // TODO monitor to be sure current is zero after it is triggered
   // printf("PACK HEARTBEAT  %2d\r\n", m_pack_silent_counter);

   m_pack_silent_counter++;
   if (m_pack_silent_counter >= PACK_SILENT_TIMEOUT_PERIODS)
   {
      if (m_pack_silent_counter == PACK_SILENT_TIMEOUT_PERIODS)
      {
         // if (m_log) *m_log << "LeafPack went silent." << std::endl;
         m_pack_silent_counter++;
      }
      m_safety_contactor.setSafeToOperate(false);
      return;
   }
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
   m_pack_silent_counter = 0;
   m_happy_poller.received(f);
   m_poller.received(f);
   m_aggregator.sink(f);

}


}
}
