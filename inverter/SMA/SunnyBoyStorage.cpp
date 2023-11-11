#include "SunnyBoyStorage.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/SMA/BatteryIdentity.hpp"
#include "can/messages/SMA/BatteryLimits.hpp"
#include "can/messages/SMA/BatteryManufacturer.hpp"
#include "can/messages/SMA/BatteryMeasurements.hpp"
#include "can/messages/SMA/BatteryName.hpp"
#include "can/messages/SMA/BatteryState.hpp"
#include "can/messages/SMA/BatterySystemInfo.hpp"
#include "can/messages/SMA/InverterCommand.hpp"
#include "can/messages/SMA/InverterIdentity.hpp"
#include <string.h>

using namespace can::messages;
using namespace can::messages::SMA;

namespace inverter {
namespace SMA {
namespace {

// TODO: figure out what to do about this as it may well be
//       a number allocated and assigned by SMA. For now
//       it's just an arbitrary number
const unsigned MANUFACTURER_ID = 2;

const unsigned INVERTER_SILENT_TIMEOUT_PERIODS = 4;

}

SunnyBoyStorage::SunnyBoyStorage(can::FrameSink& sender,
                                 core::Timer& timer,
                                 monitor::Monitor& monitor,
                                 contactor::Contactor& contactor):
      m_sender(sender),
      m_timer(timer),
      m_monitor(monitor),
      m_contactor(contactor),
      m_periodic_callback(*this, &SunnyBoyStorage::sendBatteryData),
      m_inverter_silent_counter(INVERTER_SILENT_TIMEOUT_PERIODS + 1)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, 5000,"SMAPeriodic");
}

SunnyBoyStorage::~SunnyBoyStorage()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void SunnyBoyStorage::sendBatteryData()
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

   
   m_sender.sink(BatteryMeasurements()
                 .setVoltage(m_monitor.getVoltage())
                 .setCurrent(m_monitor.getCurrent())
                 .setTemperature(m_monitor.getTemperature())
                 .setState(m_contactor.isClosed() ?
                           BatteryMeasurements::CONNECTED :
                           BatteryMeasurements::DISCONNECTED)
                 .setInverterControlFlags(0));
   m_sender.sink(BatteryState()
                 .setSocPercent(m_monitor.getSocPercent())
                 .setSohPercent(m_monitor.getSohPercent())
                 .setEnergyRemainingKwh(m_monitor.getEnergyRemainingKwh())
                 .setFullChargedEnergyKwh(m_monitor.getCapacityKwh()));
   m_sender.sink(BatteryLimits()
                 .setChargeVoltage(m_monitor.getMaxChargeVoltage())
                 .setDischargeVoltage(m_monitor.getMinDischargeVoltage())
                 .setChargeCurrent(m_monitor.getChargeCurrentLimit())
                 .setDischargeCurrent(m_monitor.getDischargeCurrentLimit()));
}

void SunnyBoyStorage::sink(const Message& message)
{
   switch (message.id())
   {
   case ID_INVERTER_COMMAND:
      process(static_cast<const InverterCommand&>(message));
      break;
   case ID_INVERTER_IDENTITY:
      process(static_cast<const InverterIdentity&>(message));
      break;
   case ID_INVERTER_MANUFACTURER:
      // not really interesting, apart from resetting
      //   the silent counter below
      break;
   default:
      return;
   }

   m_inverter_silent_counter = 0;
}


void SunnyBoyStorage::process(const InverterCommand& command)
{
   if (command.getCommand() == InverterCommand::CONNECT)
   {
      m_contactor.close();
   }
   else
   {
      m_contactor.open();
   }
}

void SunnyBoyStorage::process(const InverterIdentity&)
{
   m_sender.sink(BatterySystemInfo()
                 .setVersion(m_monitor.getSystemVersion())
                 .setCapacityKwh(m_monitor.getNominalCapacityKwh())
                 .setNumberOfModules(m_monitor.getNumberOfModules())
                 .setManufacturerId(MANUFACTURER_ID));
   m_sender.sink(BatteryIdentity()
                 .setSerialNumber(m_monitor.getSerialNumber())
                 .setManufacturingDateUnixTime(m_monitor.getManufacturingDateUnixTime()));

   sendString(m_sender, ID_BATTERY_MANUFACTURER, m_monitor.getManufacturerName());
   sendString(m_sender, ID_BATTERY_NAME, m_monitor.getBatteryName());
}


}
}
