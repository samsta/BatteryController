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

const unsigned INVERTER_SILENT_TIMEOUT_PERIODS = 6;

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
      m_inverter_silent_counter(0)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, 5000);
}

SE_PWS2::~SE_PWS2()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void SE_PWS2::periodicCallback()
{
   m_inverter_silent_counter++;

   if (m_inverter_silent_counter >= INVERTER_SILENT_TIMEOUT_PERIODS)
   {
      if (m_inverter_silent_counter == INVERTER_SILENT_TIMEOUT_PERIODS)
      {
         if (m_log) m_log->alarm("Inverter CAN bus has gone silent", __FILENAME__, __LINE__);
         m_inverter_silent_counter++;
      }
      m_contactor.open();
      return;
   }

   m_contactor.close();

   // do not send battery info unless the contractor is closed
   // contact won't close unless sufficent info has been received from the battery
   if (m_contactor.isClosed())
   {
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
      localBatteryStatus.clearBatteryStatus(BatteryStatus::BatteryStatusFlag::BSF_CHARGING_CONTACTOR_CLOSED);
      localBatteryStatus.clearBatteryStatus(BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);
      m_sender.sink(localBatteryStatus);
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

   // TODO add message when inverter starts sending again after being silent
   m_inverter_silent_counter = 0;

}

// void SE_PWS2::process(const InverterHeartbeat& command)
void SE_PWS2::process(const InverterHeartbeat& command)
{
   if (!m_first_heartbeat)
   {
      if (command.getHeartbeatValue() != (m_heartbeat_count + 1))
      {
         std::ostringstream ss;
         ss << "SE_PWS2 Inverter Heartbeat non-consecutive (" << m_heartbeat_count << ")(" << command.getHeartbeatValue() <<")";
         m_log->error(ss, __FILENAME__, __LINE__);
      }
      m_heartbeat_count = command.getHeartbeatValue();

   }
   else
   {
      m_first_heartbeat = false;
      m_heartbeat_count = command.getHeartbeatValue();
   }
}

}
}




