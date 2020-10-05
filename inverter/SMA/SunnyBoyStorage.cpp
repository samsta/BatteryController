#include "SunnyBoyStorage.hpp"

namespace inverter {
namespace SMA {

SunnyBoyStorage::SunnyBoyStorage(can::FrameSink& sender,
                                 core::Timer& timer,
                                 monitor::Monitor& monitor):
      m_sender(sender),
      m_timer(timer),
      m_monitor(monitor),
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

}

}
}
