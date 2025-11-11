/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Tesla/TeslaSlavePack.hpp>

namespace packs {
namespace Tesla {

TeslaSlavePack::TeslaSlavePack(
            char *packname,
            can::FrameSink& sender,
            core::Timer& timer,
            logging::Logger* log):
   m_pack_name(packname),
   // m_safety_shunt(packname, sender, ID_TNSY_DC_SHUNT_CTRL, log),
   // m_monitor(packname, m_safety_shunt, log),
   m_monitor(packname, log),
   m_timer(timer),
   m_message_factory(m_monitor, log, m_pack_name),
   m_aggregator(m_message_factory),
   // m_poller(sender, timer),
   // m_happy_poller(sender, timer),
   m_heartbeat_callback(*this, &TeslaSlavePack::heartbeatCallback),
   m_pack_silent_counter(0),
   m_startup_counter(0),
   m_shunt_trip_counter(0),
   m_reboot_in_process(false),
   m_shunt_fail_msg_logged(false),
   m_reboot_wait_count(0),
   m_log(log)
{
   std::string pcname; pcname.append(m_pack_name).append("_TeslaSlavePackPeriodic");
   m_timer.registerPeriodicCallback(&m_heartbeat_callback, PACK_CALLBACK_PERIOD_ms,pcname.c_str());
   char msg[1024];
   snprintf(msg,sizeof(msg), "TeslaSlavePack Initialized: %s", m_pack_name);
   if (m_log) m_log->info(msg, __FILENAME__, __LINE__);
   m_monitor.logStartupStatus();
}

TeslaSlavePack::~TeslaSlavePack()
{
   m_timer.deregisterCallback(&m_heartbeat_callback);
   char msg[1024];
   snprintf(msg,sizeof(msg), "TeslaSlavePack Distructed: %s", m_pack_name);
   if (m_log) m_log->info(msg, __FILENAME__, __LINE__);
}

void TeslaSlavePack::heartbeatCallback()
{
   // startup: battery must 'come right' in a set period of time
   // 'come right' = receive good voltage and temp readings and
   //                received one of each CAN (id) message from the battery
   switch (m_monitor.getPackStatus()) {

      case monitor::Monitor::STARTUP:
         m_monitor.logStartupStatus();
         m_startup_counter++;
         if (m_startup_counter >= MAX_PACK_STARTUP_COUNT)
         {
            m_monitor.setPackStatus(monitor::Monitor::STARTUP_FAILED);
            std::string s2;
            s2.append("TeslaSlavePack: ");
            s2.append(m_pack_name);
            s2.append(": Startup time exceeded: state changed to STARTUP_FAILED");
            if (m_log) m_log->alarm(s2, __FILENAME__,__LINE__);
         }
         break;

      case monitor::Monitor::NORMAL_OPERATION:
         // monitor the heartbeat, aka make sure we are receiving CAN messages
         // from the pack, if it goes dead, trigger the safety shunt
         m_pack_silent_counter++;
         if (m_pack_silent_counter >= PACK_SILENT_TIMEOUT_PERIODS) // && m_safety_shunt.isSafeToOperate())
         {
            std::ostringstream ss;
            ss << "TeslaSlavePack: " << m_pack_name << ": No messages received for "
                  << float(PACK_SILENT_TIMEOUT_PERIODS * PACK_CALLBACK_PERIOD_ms) / 1000.0
                  << " seconds";
            if (m_log) m_log->alarm(ss, __FILENAME__, __LINE__);
            // m_safety_shunt.setSafeToOperate(false);
            m_monitor.setPackStatus(monitor::Monitor::SHUTDOWN);
            m_monitor.updateOperationalSafety();
         }

      case monitor::Monitor::STARTUP_FAILED:
      case monitor::Monitor::SHUNT_ACTIVIATED:
      case monitor::Monitor::SHUNT_ACT_FAILED:
      case monitor::Monitor::SHUTDOWN:
      default:
         break;
   }
   
   // if (!m_safety_shunt.isSafeToOperate())
   // {
   //    m_shunt_trip_counter++;
   //    // we only get here if the shunt is tripped
   //    // wait a few counts before checking the current
   //    if (m_shunt_trip_counter < SHUNT_TRIP_COUNT) return;

   //    m_shunt_trip_counter = SHUNT_TRIP_COUNT + 1;
   //    // check the current is zero when the shunt has tripped
   //    // actually, check that it is a small value as the current measurement is not accurate
   //    if (m_monitor.getCurrent() > MAX_SHUNT_OPEN_CURRENT && !m_shunt_fail_msg_logged)
   //    {
   //       m_monitor.setPackStatus(monitor::Monitor::SHUNT_ACT_FAILED);
   //       std::ostringstream ss;
   //       ss << "TeslaSlavePack: " << m_pack_name << ": SHUNT ALREADY TRIPPED BUT CURRENT NOT ZERO ("
   //           << std::fixed << std::setprecision(2) << m_monitor.getCurrent() << ")  CHECK SHUNT OPERATION.  THIS LIKELY DUE TO LOSS OF COMMS WITH BATTERY.";
   //       if (m_log) m_log->error(ss, __FILENAME__, __LINE__);
   //       m_safety_shunt.setSafeToOperate(false);
   //       m_monitor.updateOperationalSafety();
   //       m_shunt_fail_msg_logged = true;
   //    }
   //    // if the current has gone to near zero, release the shunt trip relay
   //    // else // (m_monitor.getCurrent() < MAX_SHUNT_OPEN_CURRENT)
   //    // {
   //    //    std::ostringstream ss;
   //    //    ss << "TeslaSlavePack: " << m_pack_name << ": shunt trip relay de-energized";
   //    //    if (m_log) m_log->info(ss, __FILENAME__, __LINE__);
   //    //    // m_safety_shunt.setSafeToOperate(true);
   //    //    // m_monitor.updateOperationalSafety();
   //    // }
   // }
}

monitor::Monitor& TeslaSlavePack::getMonitor()
{
   return m_monitor;
}

// contactor::Contactor& TeslaSlavePack::getContactor()
// {
//    return m_safety_shunt;
// }

char* TeslaSlavePack::getPackName()
{
   return m_pack_name;
}

void TeslaSlavePack::sink(const can::DataFrame& f)
{
   m_pack_silent_counter = 0;
   // m_happy_poller.received(f);
   // m_poller.received(f);
   m_aggregator.sink(f);

}


}
}
