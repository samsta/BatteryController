#include "SE-PWS2.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/SINEX/InverterHeartbeat.hpp"
#include "can/messages/SINEX/BatteryLimitsOne.hpp"
#include "can/messages/SINEX/BatteryLimitsTwo.hpp"
#include "can/messages/SINEX/BatteryStatus.hpp"

#include <string.h>

using namespace can::messages;
using namespace can::messages::SINEX;

namespace inverter {
namespace SINEX {
namespace {

const unsigned int PERIODIC_CALLBACK_ms = 500;
const unsigned int INVERTER_SILENT_TIMEOUT_PERIODS = 30 * 1000 / PERIODIC_CALLBACK_ms;

BatteryStatus localBatteryStatus(BatteryStatus::BatteryStatusFlag::BSF_CLEAR);
}

SE_PWS2::SE_PWS2(can::FrameSink& sender,
                     core::Timer& timer,
                     monitor::Monitor& monitor,
                     contactor::Contactor& contactor,
                     logging::Logger *log):
      m_sender(sender),
      m_timer(timer),
      m_monitor(monitor),
      m_contactor(contactor),
      m_log(log),
      m_periodic_callback(*this, &SE_PWS2::periodicCallback),
      m_heartbeat_count(0),
      m_first_heartbeat(true),
      m_inverter_silent_counter(0),
      m_hb_non_consec(false),
      m_contactor_closed(false)
{
   // send inverter messages at 500ms intervals
   m_timer.registerPeriodicCallback(&m_periodic_callback, PERIODIC_CALLBACK_ms);
}

SE_PWS2::~SE_PWS2()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void SE_PWS2::periodicCallback()
{
   // this is called every PERIODIC_CALLBACK_ms (500ms at time of writing)
   m_inverter_silent_counter++;

   if (m_inverter_silent_counter >= INVERTER_SILENT_TIMEOUT_PERIODS)
   {
      // if the inverter has gone silent, open the contactor
      if (m_inverter_silent_counter == INVERTER_SILENT_TIMEOUT_PERIODS)
      {
         if (m_log) m_log->alarm("Inverter CAN bus has gone silent", __FILENAME__, __LINE__);
         m_inverter_silent_counter++;
      }
      m_contactor.open();
   }
   else 
   {
      // contactor won't close unless sufficent info has been received from the battery
      m_contactor.close();

      // do not send battery info unless the contractor is closed
      if (m_contactor.isClosed())
      {
         m_contactor_closed = true;
         m_sender.sink(BatteryLimitsOne()
                     .setMaxChargeCurrent (m_monitor.getChargeCurrentLimit())
                     .setMaxDischargeCurrent(m_monitor.getDischargeCurrentLimit())
                     .setTotalVoltage(m_monitor.getVoltage())
                     .setTotalCurrent(m_monitor.getCurrent()));

         m_sender.sink(BatteryLimitsTwo()
                     .setMaxChargingVoltage (m_monitor.getMaxChargeVoltage())
                     .setSOCPercent(m_monitor.getSocPercent())
                     .setSOHPercent(m_monitor.getSohPercent()));

         localBatteryStatus.setBatteryStatus(BatteryStatus::BatteryStatusFlag::BSF_CHARGING_CONTACTOR_CLOSED);
         localBatteryStatus.setBatteryStatus(BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);
         m_sender.sink(localBatteryStatus);

         // // TODO replace dummy values
         // m_sender.sink(BatteryCellVoltInfo()
         //       .setMaxSingleCellVoltage(3.980)
         //       .setMinSingleCellVoltage(3.978)
         //       .setMaxCellVoltageNumber(1)
         //       .setMinCellVoltageNumber(2));

         // // TODO replace dummy values
         // m_sender.sink(BatteryCellTempInfo()
         //       .setMaxSingleCellTemp(16)
         //       .setMinSingleCellTemp(14)
         //       .setMaxCellTempNumber(3)
         //       .setMinCellTempNumber(4));

         // // TODO replace dummy values
         // m_sender.sink(BatteryModVoltInfo()
         //       .setMaxSingleModuleVoltage(2 * 3.980)
         //       .setMinSingleModuleVoltage(2 * 3.978)
         //       .setMaxModuleVoltageNumber(1)
         //       .setMinModuleVoltageNumber(2));

         // // TODO replace dummy values
         // m_sender.sink(BatteryModTempInfo()
         //       .setMaxSingleModuleTemp(16)
         //       .setMinSingleModuleTemp(14)
         //       .setMaxModuleTempNumber(3)
         //       .setMinModuleTempNumber(4));

         // m_sender.sink(BatteryForbidden());
      }
      else // contactor is open
      {
         if (m_contactor_closed)
         {
            // only send this once, when the contactor opens
            m_contactor_closed =  false;
            localBatteryStatus.clearBatteryStatus(BatteryStatus::BatteryStatusFlag::BSF_CHARGING_CONTACTOR_CLOSED);
            localBatteryStatus.clearBatteryStatus(BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);
            m_sender.sink(localBatteryStatus);
         }
      }
   }
}

void SE_PWS2::sink(const Message& message)
{
   switch (message.id())
   {
   case ID_INVERTER_HEARTBEAT:
      process(static_cast<const InverterHeartbeat&>(message));
      break;
   default:
      // TODO:log alarm message?
      return;
   }
}

void SE_PWS2::process(const InverterHeartbeat& command)
{
   m_hb_non_consec = false;

   if (!m_first_heartbeat)
   {
      if (command.getHeartbeatValue() != uint16_t(m_heartbeat_count + 1))
      {
         m_hb_non_consec = true;
         std::ostringstream ss;
         ss << "SE_PWS2 Inverter Heartbeat value non-consecutive (" << m_heartbeat_count << ")(" << command.getHeartbeatValue() <<")";
         if (m_log) m_log->error(ss, __FILENAME__, __LINE__);
      }
      m_heartbeat_count = command.getHeartbeatValue();
   }
   else
   {
      m_first_heartbeat = false;
      m_heartbeat_count = command.getHeartbeatValue();
   }

   // TODO add message when inverter starts sending again after being silent
   m_inverter_silent_counter = 0;
}

uint16_t SE_PWS2::getHeartbeatValue()
{
   return m_heartbeat_count;
}

bool SE_PWS2::getHbNonConsec()
{
   return m_hb_non_consec;
}

}
}


