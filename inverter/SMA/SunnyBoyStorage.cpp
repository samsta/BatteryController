#include "SunnyBoyStorage.hpp"
#include "can/messages/SMA/BatteryIdentity.hpp"
#include "can/messages/SMA/BatteryName.hpp"
#include "can/messages/SMA/BatteryManufacturer.hpp"
#include "can/messages/SMA/BatteryState.hpp"
#include "can/messages/SMA/BatterySystemInfo.hpp"
#include "can/messages/SMA/InverterCommand.hpp"
#include <string.h>

using namespace can::messages::SMA;

namespace inverter {
namespace SMA {
namespace {

// TODO: figure out what to do about this as it may well be
//       a number allocated and assigned by SMA. For now
//       it's just an arbitrary number
const unsigned MANUFACTURER_ID = 2;

}

SunnyBoyStorage::SunnyBoyStorage(can::FrameSink& sender,
                                 core::Timer& timer,
                                 monitor::Monitor& monitor,
                                 contactor::Contactor& contactor):
      m_sender(sender),
      m_timer(timer),
      m_monitor(monitor),
      m_contactor(contactor),
      m_periodic_callback(*this, &SunnyBoyStorage::sendBatteryData)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, 5000);
}

SunnyBoyStorage::~SunnyBoyStorage()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void SunnyBoyStorage::sendBatteryData()
{
   m_sender.sink(BatteryState()
                 .setSocPercent(m_monitor.getSocPercent())
                 .setSohPercent(m_monitor.getSohPercent())
                 .setEnergyRemainingKwh(m_monitor.getEnergyRemainingKwh())
                 .setFullChargedEnergyKwh(m_monitor.getCapacityKwh()));
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

void SunnyBoyStorage::process(const can::messages::SMA::InverterIdentity&)
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
