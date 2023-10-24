/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryLimitsTwo.hpp"
#include "can/messages/SINEX/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SINEX {

BatteryLimitsTwo::BatteryLimitsTwo():
   StandardDataFrame(ID_BATTERY_LIMITS_TWO, "0000000000000000")
{
}

BatteryLimitsTwo::BatteryLimitsTwo(float soc_percent,
                     float soh_percent,
                     float max_charge_voltage):
   StandardDataFrame(ID_BATTERY_LIMITS_TWO, "0000000000000000")
{
   setSOCPercent(soc_percent);
   setSOHPercent(soh_percent);
   setMaxChargingVoltage(max_charge_voltage);
}

BatteryLimitsTwo& BatteryLimitsTwo::setSOCPercent(float soc)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(soc, 10), LSB_FIRST);
   return *this;
}

BatteryLimitsTwo& BatteryLimitsTwo::setSOHPercent(float soh)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(soh, 10), LSB_FIRST);
   return *this;
}

BatteryLimitsTwo& BatteryLimitsTwo::setMaxChargingVoltage(float voltage)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(voltage, 10), LSB_FIRST);
   return *this;
}

}
}
}
