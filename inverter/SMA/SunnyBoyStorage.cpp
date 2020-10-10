#include "SunnyBoyStorage.hpp"
#include "can/messages/SMA/BatteryState.hpp"
#include "can/messages/SMA/InverterCommand.hpp"

using namespace can::messages::SMA;

namespace inverter {
namespace SMA {

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
      m_contactor.open();
   }
   else
   {
      m_contactor.close();
   }
}


}
}
