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
   m_battery_status(can::messages::Tesla::TSBatteryStatus::STARTUP)
{
}

#define STATUS_ALL_GOOD 0xAAAA
#define STATUS_EXCESSIVE_CONSECUTIVE_FAILED_READS 0xDDDD
#define STATUS_BAD_PACKET_RATE 0xEEEE
#define STATUS_NO_MODULES 0xFFFF

TSBatteryStatus::TSBatteryStatus(const DataFrame& frame):
   Message(ID_TS_BATTERY_STATUS, GROUP_NONE),
   m_battery_status(can::messages::Tesla::TSBatteryStatus::STARTUP)
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   uint32_t battery_status = frame.getUnsignedLong(0, 2);
   switch ( battery_status )
   {
      case STATUS_ALL_GOOD:
         m_battery_status = can::messages::Tesla::TSBatteryStatus::OK;
         break;

      case STATUS_EXCESSIVE_CONSECUTIVE_FAILED_READS:
         m_battery_status = can::messages::Tesla::TSBatteryStatus::EXCESSS_CONSECUTIVE_READ_ERRORS;
         break;

      case STATUS_BAD_PACKET_RATE:
            m_battery_status = can::messages::Tesla::TSBatteryStatus::EXECESS_READ_ERROR_RATE;
         break;

      case STATUS_NO_MODULES:
         m_battery_status = can::messages::Tesla::TSBatteryStatus::ZERO_MODULES;
         break;

      default:
         m_battery_status = can::messages::Tesla::TSBatteryStatus::UNRECOGNIZED;
         break;
   }
   
   setValid();
}

can::messages::Tesla::TSBatteryStatus::Battery_Status TSBatteryStatus::getBatteryStatus() const
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




