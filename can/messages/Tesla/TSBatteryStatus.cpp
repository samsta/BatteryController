/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "TSBatteryStatus.hpp"
#include "logging/Hex.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Tesla {
namespace {

}

TSBatteryStatus::TSBatteryStatus():
   Message(ID_TS_BATTERY_STATUS, GROUP_NONE),
   m_battery_status(3)
{
}

TSBatteryStatus::TSBatteryStatus(const DataFrame& frame):
   Message(ID_TS_BATTERY_STATUS, GROUP_NONE),
   m_battery_status()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   uint32_t battery_status = frame.getUnsignedLong(0, 2);
   if (battery_status == 0xAAAA) {
      m_battery_status = 1;
   }
   else if (battery_status == 0x5555) {
      m_battery_status = 0;
   }
   else {
      m_battery_status = 2;
   }
   
   setValid();
}

uint32_t TSBatteryStatus::getBatteryStatus() const
{
   return m_battery_status;
}

void TSBatteryStatus::toStream(logging::ostream& os) const
{
   os << "TSBatteryStatus: 0x" << logging::Hex(ID_TS_BATTERY_STATUS) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "BatStatus=" << m_battery_status;
}


}
}
}




