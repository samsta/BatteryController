/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryLimitsOne.hpp"
#include "can/messages/SINEX/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SINEX {

BatteryLimitsOne::BatteryLimitsOne():
   StandardDataFrame(ID_BATTERY_LIMITS_ONE, "0000000000000000")
{
}

BatteryLimitsOne::BatteryLimitsOne(float max_charge_current,
                             float max_discharge_current,
                             float total_voltage,
                             float total_current):
   StandardDataFrame(ID_BATTERY_LIMITS_ONE, "0000000000000000")
{
   setMaxChargeCurrent(max_charge_current);
   setMaxDischargeCurrent(max_discharge_current);
   setTotalVoltage(total_voltage);
   setTotalCurrent(total_current);
}

BatteryLimitsOne& BatteryLimitsOne::setMaxChargeCurrent(float current)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(current, 10), LSB_FIRST);
   return *this;
}

BatteryLimitsOne& BatteryLimitsOne::setMaxDischargeCurrent(float current)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(current, 10), LSB_FIRST);
   return *this;
}

BatteryLimitsOne& BatteryLimitsOne::setTotalVoltage(float voltage)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(voltage, 10), LSB_FIRST);
   return *this;
}

BatteryLimitsOne& BatteryLimitsOne::setTotalCurrent(float current)
{
   setUnsignedShort(6, limitScaledToUnsignedShort(current, 10), LSB_FIRST);
   return *this;
}

}
}
}
