/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafPack.hpp>

namespace packs {
namespace Nissan {

LeafPack::LeafPack(
            char *packname,
            can::FrameSink& sender,
            core::Timer& timer,
            logging::Logger* log):
   m_pack_name(packname),
   m_safety_shunt(packname, sender, ID_TNSY_DC_SHUNT_CTRL, log),
   m_power_relay(packname, sender, ID_TNSY_LBC_PWR_RLY, log),
   m_monitor(packname, m_safety_shunt, log),
   m_timer(timer),
   m_message_factory(m_monitor, log),
   m_aggregator(m_message_factory),
   m_poller(sender, timer),
   m_happy_poller(sender, timer),
   m_heartbeat_callback(*this, &LeafPack::heartbeatCallback),
   m_pack_silent_counter(0),
   m_reboot_in_process(false),
   m_reboot_wait_count(0),
   m_log(log)
{
   m_timer.registerPeriodicCallback(&m_heartbeat_callback, PACK_CALLBACK_PERIOD_ms);
   char msg[1024];
   sprintf(msg, "LeafPack Initialized: %s", m_pack_name);
   if (m_log) m_log->info(msg, __FILENAME__, __LINE__);
}

LeafPack::~LeafPack()
{
   m_timer.deregisterCallback(&m_heartbeat_callback);
   char msg[1024];
   sprintf(msg, "LeafPack Distructed: %s", m_pack_name);
   if (m_log) m_log->info(msg, __FILENAME__, __LINE__);
}

void LeafPack::heartbeatCallback()
{
   // monitor the heartbeat, aka make sure we are receiving CAN messages
   // from the pack, if it goes dead, trigger the safety shunt
   // TODO monitor to be sure current is zero after it is triggered

   m_pack_silent_counter++;
   if (m_pack_silent_counter >= PACK_SILENT_TIMEOUT_PERIODS && m_safety_shunt.isSafeToOperate())
   {
      std::ostringstream ss;
      ss << "LeafPack: " << m_pack_name << ": No CAN messages received for "
            << float(PACK_SILENT_TIMEOUT_PERIODS * PACK_CALLBACK_PERIOD_ms) / 1000.0
            << " seconds";
      m_log->alarm(ss, __FILENAME__, __LINE__);
      m_safety_shunt.setSafeToOperate(false);
      m_monitor.updateOperationalSafety();
   }
   else if (!m_safety_shunt.isSafeToOperate())
   {
      // check the current is zero when the shut is triggered (actually, check that it is a small value as
      // the current measurement is not accurate)
      float x =1.1;
      if (m_monitor.getCurrent() > MAX_SHUNT_OPEN_CURRENT)
      {
         std::ostringstream ss;
         ss << "LeafPack: " << m_pack_name << ": SHUNT ALREADY TRIGGERED BUT CURRENT NOT ZERO.  CHECK SHUNT OPERATION.";
         m_log->error(ss, __FILENAME__, __LINE__);
         m_safety_shunt.setSafeToOperate(false);
         m_monitor.updateOperationalSafety();
      }
   }

   // check failsafe status, see if battery needs to be power cycled (aka reboot!)
   // reboot is the only way to reset failsafe status
   m_reboot_wait_count++;
   if ((m_monitor.getFailsafeStatus() & 0b100)
         && m_reboot_wait_count > REBOOT_WAIT_PERIODS
         && m_monitor.getPackStatus() == monitor::Monitor::NORMAL_OPERATION)
   {
      m_reboot_wait_count = 0;
      m_reboot_in_process = true;
      std::ostringstream ss;
      ss << "LeafPack: " << m_pack_name << ": Failsafe Status indicates Pack needs a reboot";
      m_log->alarm(ss, __FILENAME__, __LINE__);
      m_power_relay.setState(contactor::Nissan::TeensyRelay::ENERGIZED);
   }
   else if (m_reboot_in_process)
   {
      m_reboot_in_process = false;
      std::ostringstream ss;
      ss << "LeafPack: " << m_pack_name << ": Reboot complete, cannot reboot again for "
            << REBOOT_WAIT_PERIODS * PACK_CALLBACK_PERIOD_ms / 1000 << " seconds";
      m_log->alarm(ss, __FILENAME__, __LINE__);
      m_power_relay.setState(contactor::Nissan::TeensyRelay::DE_ENERGIZED);
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
