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
#include "can/messages/TSUN/InverterChargeDischargeCmd.hpp"
#include "can/messages/TSUN/InverterInfoRequest.hpp"
#include "can/messages/TSUN/InverterSleepAwakeCmd.hpp"

#include <string.h>

using namespace can::messages;
using namespace can::messages::TSUN;

namespace inverter {
namespace TSUN {
namespace {


const unsigned INVERTER_SILENT_TIMEOUT_PERIODS = 2;

BatteryStatus localBatteryStatus(BatteryStatus::BASIC_STATUS_IDLE);

}

TSOL_H50K::TSOL_H50K(can::FrameSink& sender,
                                 core::Timer& timer,
                                 monitor::Monitor& monitor,
                                 contactor::Contactor& contactor):
      m_sender(sender),
      m_timer(timer),
      m_monitor(monitor),
      m_contactor(contactor),
      m_periodic_callback(*this, &TSOL_H50K::periodicCallback),
      m_inverter_silent_counter(INVERTER_SILENT_TIMEOUT_PERIODS + 1)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, 5000);
}

TSOL_H50K::~TSOL_H50K()
{
   m_timer.deregisterCallback(&m_periodic_callback);

   // I didn't see an 'open' in m_contactor destructor so I put one here. open for discussion
   m_contactor.open();
}

void TSOL_H50K::periodicCallback()
{
   if (m_inverter_silent_counter >= INVERTER_SILENT_TIMEOUT_PERIODS)
   {
      if (m_inverter_silent_counter == INVERTER_SILENT_TIMEOUT_PERIODS)
      {
         std::cout << "Inverter went silent, so I'll stop spamming it" << std::endl;
         m_inverter_silent_counter++;
      }
      m_contactor.open();
      return;
   }
   m_inverter_silent_counter++;
}

void TSOL_H50K::sink(const Message& message)
{
   switch (message.id())
   {
   case ID_INVERTER_INFO_REQUEST:
      process(static_cast<const InverterInfoRequest&>(message));
      break;
   default:
      return;
   }

   m_inverter_silent_counter = 0;
}


void TSOL_H50K::process(const InverterInfoRequest& command)
{
   if (command.getInfoType() == InverterInfoRequest::ENSEMBLE)
   {
      m_contactor.close();
   } // FOR TESTING THE TESTING

      // send Ensemble Information
      m_sender.sink(BatteryInfo()
                  .setPileVoltage(m_monitor.getVoltage())
                  .setPileCurrent(m_monitor.getCurrent())
                  .setBMS2ndTemp(m_monitor.getTemperature())
                  .setSOC(unsigned(m_monitor.getSocPercent() +0.5))
                  .setSOH(unsigned(m_monitor.getSohPercent() +0.5)));

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
//   }
//   else if (command.getInfoType() == InverterInfoRequest::SYSTEM_EQUIPMENT)
//   {
//      // send System Equipment Info
//      // have never seen this data requested by the inverter
//
//   }
   // TODO else some kind of error reporting?
}


//void TSOL_H50K::process(const can::messages::SMA::InverterIdentity&)
//{
//   m_sender.sink(BatterySystemInfo()
//                 .setVersion(m_monitor.getSystemVersion())
//                 .setCapacityKwh(m_monitor.getNominalCapacityKwh())
//                 .setNumberOfModules(m_monitor.getNumberOfModules())
//                 .setManufacturerId(MANUFACTURER_ID));
//   m_sender.sink(BatteryIdentity()
//                 .setSerialNumber(m_monitor.getSerialNumber())
//                 .setManufacturingDateUnixTime(m_monitor.getManufacturingDateUnixTime()));
//
//   sendString(m_sender, ID_BATTERY_MANUFACTURER, m_monitor.getManufacturerName());
//   sendString(m_sender, ID_BATTERY_NAME, m_monitor.getBatteryName());
//}


}
}
