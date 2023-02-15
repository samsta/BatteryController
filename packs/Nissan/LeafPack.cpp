/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafPack.hpp>

namespace packs {
namespace Nissan {

LeafPack::LeafPack( char *packname,
            can::FrameSink& sender,
            core::Timer& timer,
            logging::Logger* log):
   m_packname(packname),
   m_safety_shunt(sender, ID_TNSY_DC_SHUNT_CTRL),
   m_monitor(m_safety_shunt),
   m_timer(timer),
   m_message_factory(m_monitor, log),
   m_aggregator(m_message_factory),
   m_poller(sender, timer),
   m_happy_poller(sender, timer),
   m_heartbeat_callback(*this, &LeafPack::heartbeatCallback),
   m_pack_silent_counter(0),
   m_log(log)
{
   m_timer.registerPeriodicCallback(&m_heartbeat_callback, PACK_CALLBACK_PERIOD_ms);
   char msg[1024];
   sprintf(msg, "Pack Initialized: %s", m_packname);
   if (m_log) m_log->info(msg, __FILENAME__, __LINE__);
}

LeafPack::~LeafPack()
{
   m_timer.deregisterCallback(&m_heartbeat_callback);
}

void LeafPack::heartbeatCallback()
{
   // monitor the heartbeat, if it goes dead, trigger the safety shunt
   // TODO monitor to be sure current is zero after it is triggered

   m_pack_silent_counter++;
   if (m_pack_silent_counter >= PACK_SILENT_TIMEOUT_PERIODS)
   {
      if (m_pack_silent_counter == PACK_SILENT_TIMEOUT_PERIODS)
      {
         if (m_log) {
            char text[1024];
            sprintf(text, "LeafPack %s: No CAN messages received for %.1f seconds",
                     "BAT_NAME",float(PACK_SILENT_TIMEOUT_PERIODS * PACK_CALLBACK_PERIOD_ms) / 1000.0);
            m_log->error(text);
         }
         m_pack_silent_counter++;
      }
      m_safety_shunt.setSafeToOperate(false);
      m_monitor.updateOperationalSafety();
      return;
   }

}

monitor::Monitor& LeafPack::getMonitor()
{
   return m_monitor;
}

contactor::Contactor& LeafPack::getContactor()
{
   return m_safety_shunt;
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
