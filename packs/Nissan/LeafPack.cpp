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
   m_message_factory(m_monitor, log, m_pack_name),
   m_aggregator(m_message_factory),
   m_poller(sender, timer),
   m_happy_poller(sender, timer),
   m_heartbeat_callback(*this, &LeafPack::heartbeatCallback),
   m_pack_silent_counter(0),
   m_startup_counter(0),
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
   // startup: battery must 'come right' in a set period of time
   // 'come right' = receive good voltage and temp readings and
   //                received on of each CAN (id) message from the battery
   switch (m_monitor.getPackStatus()) {

      case monitor::Monitor::STARTUP:
         m_monitor.logStartupStatus();
         m_startup_counter++;
         if (m_startup_counter >= MAX_PACK_STARTUP_COUNT)
         {
            m_safety_shunt.setSafeToOperate(false);
            m_monitor.setPackStatus(monitor::Monitor::SHUNT_ACTIVIATED);
            std::string s2;
            s2.append("LeafPack: ");
            s2.append(m_pack_name);
            s2.append(": Startup time exceeded: SHUNT ACTIVIATED");
            if (m_log) m_log->alarm(s2, __FILENAME__,__LINE__);
            if (m_startup_counter >= m_pack_silent_counter)
            {
               std::string s3;
               s3.append("LeafPack: ");
               s3.append(m_pack_name);
               s3.append(": Did not receive a complete set of CAN messages during startup.");
               if (m_log) m_log->alarm(s3, __FILENAME__,__LINE__);
            }
            else
            {
               std::string s1;
               s1.append("LeafPack: ");
               s1.append(m_pack_name);
               s1.append(":  Alarm Condition(s) Present:");
               s1.append(m_monitor.getAlarmConditionText());
               if (m_log) m_log->alarm(s1, __FILENAME__,__LINE__);
            }
         }
         break;

      case monitor::Monitor::NORMAL_OPERATION:
      case monitor::Monitor::STARTUP_FAILED:
      case monitor::Monitor::SHUNT_ACTIVIATED:
      case monitor::Monitor::SHUNT_ACT_FAILED:
      case monitor::Monitor::SHUTDOWN:
      default:
         break;
   }

   // THIS CODE NEEDS REVISITING AS THERE SHOULD ONLY BE ONE PLACE WHERE
   // THE SHUNT IS ACTIVATED AFTER THE STATE CHANGED TO SHUNT_ACTIVATED.

   // THERE SHOULD BE A SEPARTE CASE FOR SHUNT_ACTIVATED AT THE BOTTOM OF THIS
   // SECTION WHICH DOES THE JOB IMMEDIDATELY AFTER THE STATE IS CHANGED.
   
   // monitor the heartbeat, aka make sure we are receiving CAN messages
   // from the pack, if it goes dead, trigger the safety shunt
   m_pack_silent_counter++;
   if (m_pack_silent_counter >= PACK_SILENT_TIMEOUT_PERIODS && m_safety_shunt.isSafeToOperate())
   {
      std::ostringstream ss;
      ss << "LeafPack: " << m_pack_name << ": No CAN messages received for "
            << float(PACK_SILENT_TIMEOUT_PERIODS * PACK_CALLBACK_PERIOD_ms) / 1000.0
            << " seconds";
      if (m_log) m_log->alarm(ss, __FILENAME__, __LINE__);
      m_safety_shunt.setSafeToOperate(false);
      m_monitor.updateOperationalSafety();
   }
   else if (!m_safety_shunt.isSafeToOperate())
   {
      // check the current is zero when the shunt is tripped
      // actually, check that it is a small value as the current measurement is not accurate
      if (m_monitor.getCurrent() > MAX_SHUNT_OPEN_CURRENT)
      {
         m_monitor.setPackStatus(monitor::Monitor::SHUNT_ACT_FAILED);
         std::ostringstream ss;
         ss << "LeafPack: " << m_pack_name << ": SHUNT ALREADY TRIPPED BUT CURRENT NOT ZERO.  CHECK SHUNT OPERATION.";
         if (m_log) m_log->error(ss, __FILENAME__, __LINE__);
         m_safety_shunt.setSafeToOperate(false);
         m_monitor.updateOperationalSafety();
      }
      // if the current has gone to near zero, release the shunt trip relay
      // else // (m_monitor.getCurrent() < MAX_SHUNT_OPEN_CURRENT)
      // {
      //    std::ostringstream ss;
      //    ss << "LeafPack: " << m_pack_name << ": shunt trip relay de-energized";
      //    if (m_log) m_log->info(ss, __FILENAME__, __LINE__);
      //    // m_safety_shunt.setSafeToOperate(true);
      //    // m_monitor.updateOperationalSafety();
      // }
   }

   // check failsafe status, see if battery needs to be power cycled (aka reboot!)
   // reboot is the only way to reset failsafe status
   // possible future issue https://github.com/samsta/BatteryController/issues/17
   m_reboot_wait_count++;
   if ((m_monitor.getFailsafeStatus() & 0b100)
         && m_reboot_wait_count > REBOOT_WAIT_PERIODS
         && m_monitor.getPackStatus() == monitor::Monitor::NORMAL_OPERATION)
   {
      m_reboot_wait_count = 0;
      m_reboot_in_process = true;
      std::ostringstream ss;
      ss << "LeafPack: " << m_pack_name << ": Failsafe Status indicates Pack needs a reboot";
      if (m_log) m_log->alarm(ss, __FILENAME__, __LINE__);
      m_power_relay.setState(contactor::Nissan::TeensyRelay::ENERGIZED);
   }
   else if (m_reboot_in_process && (m_reboot_wait_count > REBOOT_POWERDOWN_PERIODS))
   {
      m_reboot_in_process = false;
      std::ostringstream ss;
      ss << "LeafPack: " << m_pack_name << ": Reboot complete, cannot reboot again for "
            << REBOOT_WAIT_PERIODS * PACK_CALLBACK_PERIOD_ms / 1000 << " seconds";
      if (m_log) m_log->alarm(ss, __FILENAME__, __LINE__);
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

char* LeafPack::getPackName()
{
   return m_pack_name;
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
