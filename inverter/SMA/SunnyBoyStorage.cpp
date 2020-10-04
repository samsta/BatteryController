#include "SunnyBoyStorage.hpp"

namespace inverter {
namespace SMA {

SunnyBoyStorage::SunnyBoyStorage(can::FrameSink& sender,
                                 core::Timer& timer,
                                 monitor::Monitor& monitor):
      m_sender(sender),
      m_timer(timer),
      m_monitor(monitor)
{
}


}
}
