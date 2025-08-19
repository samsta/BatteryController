#include "TSOL-H50K.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/TSUN/BatteryCellTempInfo.hpp"
#include "can/messages/TSUN/BatteryCellVoltInfo.hpp"
#include "can/messages/TSUN/BatteryForbidden.hpp"
#include "can/messages/TSUN/BatteryInfo.hpp"
#include "can/messages/TSUN/BatteryLimits.hpp"
#include "can/messages/TSUN/BatteryModTempInfo.hpp"
#include "can/messages/TSUN/BatteryModVoltInfo.hpp"
#include "can/messages/TSUN/BatteryStatus.hpp"
#include "can/messages/TSUN/BatteryHWSWVersion.hpp"
#include "can/messages/TSUN/BatteryModCapacity.hpp"
#include "can/messages/TSUN/InverterChargeDischargeCmd.hpp"
#include "can/messages/TSUN/InverterInfoRequest.hpp"
#include "can/messages/TSUN/InverterSleepAwakeCmd.hpp"

#include <string.h>

using namespace can::messages;
using namespace can::messages::TSUN;

namespace inverter {
namespace TSUN {
namespace {


const unsigned INVERTER_SILENT_TIMEOUT_PERIODS = 25;

BatteryStatus localBatteryStatus(BatteryStatus::BASIC_STATUS_IDLE);

}

TSOL_H50K::TSOL_H50K(can::FrameSink& sender,
                     core::Timer& timer,
                     monitor::Monitor& monitor,
                     contactor::Contactor& contactor,
                     logging::Logger *log):
      m_sender(sender),
      m_timer(timer),
      m_monitor(monitor),
      m_contactor(contactor),
      m_log(log),
      m_periodic_callback(*this, &TSOL_H50K::periodicCallback),
      m_inverter_silent_counter(0)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, 5000,"TSUNPeriodic");
}

TSOL_H50K::~TSOL_H50K()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void TSOL_H50K::periodicCallback()
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
}

void TSOL_H50K::sink(const Message& message)
{
   switch (message.id())
   {
   case ID_INVERTER_INFO_REQUEST:
      process(static_cast<const InverterInfoRequest&>(message));
      break;
   default:
      char info[500];
      snprintf(info, sizeof(info), "Unprocessed CAN message from Inverter: %8X", message.id());
      if (m_log) m_log->info(info,__FILENAME__,__LINE__);
      return;
   }

   // TODO add message when inverter starts sending again after being silent
   m_inverter_silent_counter = 0;
   
}


void TSOL_H50K::process(const InverterInfoRequest& command)
{
   if (command.getInfoType() == InverterInfoRequest::ENSEMBLE)
   {
      m_contactor.close();

      // do not send battery info unless the contractor is closed
      // contact won't close unless sufficent info has been received from the battery
      if (m_contactor.isClosed()) {

    	  // send Ensemble Information
          m_sender.sink(BatteryInfo()
                      .setPileVoltage(m_monitor.getVoltage())
                      .setPileCurrent(m_monitor.getCurrent())
                      .setBMS2ndTemp(m_monitor.getTemperature())
                      .setSOC(unsigned(m_monitor.getSocPercent()))
                      .setSOH(unsigned(m_monitor.getSohPercent())));

          m_sender.sink(BatteryLimits()
                        .setChargeVoltage(m_monitor.getMaxChargeVoltage())
                        .setDischargeVoltage(m_monitor.getMinDischargeVoltage())
                        .setChargeCurrent(m_monitor.getChargeCurrentLimit())
                        .setDischargeCurrent(m_monitor.getDischargeCurrentLimit()));

          // TODO replace dummy values
          m_sender.sink(BatteryCellVoltInfo()
                .setMaxSingleCellVoltage(3.980)
                .setMinSingleCellVoltage(3.978)
                .setMaxCellVoltageNumber(1)
                .setMinCellVoltageNumber(2));

          // TODO replace dummy values
          m_sender.sink(BatteryCellTempInfo()
                .setMaxSingleCellTemp(16)
                .setMinSingleCellTemp(14)
                .setMaxCellTempNumber(3)
                .setMinCellTempNumber(4));

          m_sender.sink(localBatteryStatus);

          // TODO replace dummy values
          m_sender.sink(BatteryModVoltInfo()
                .setMaxSingleModuleVoltage(2 * 3.980)
                .setMinSingleModuleVoltage(2 * 3.978)
                .setMaxModuleVoltageNumber(1)
                .setMinModuleVoltageNumber(2));

          // TODO replace dummy values
          m_sender.sink(BatteryModTempInfo()
                .setMaxSingleModuleTemp(16)
                .setMinSingleModuleTemp(14)
                .setMaxModuleTempNumber(3)
                .setMinModuleTempNumber(4));

          m_sender.sink(BatteryForbidden());
      }

   }
   else if (command.getInfoType() == InverterInfoRequest::SYSTEM_EQUIPMENT)
   {
      // do not send battery info unless the contractor is closed
      // contact won't close unless sufficent info has been received from the battery
      if (m_contactor.isClosed()) {      

         // send System Equipment Info
         m_sender.sink(BatteryHWSWVersion());

         m_sender.sink(BatteryModCapacity()
               .setTotalCellAmount(120)
               .setModulesInSeries(4)
               .setCellsPerModule(30)
               .setVoltageLevel(384)
               // Ah = Wh / V
               .setAhCapacity(37));
      }
   }
   // TODO else some kind of error reporting?
}

}
}
