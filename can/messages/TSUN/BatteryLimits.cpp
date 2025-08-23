/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryLimits.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryLimits::BatteryLimits():
   StandardDataFrame(ID_BATTERY_LIMITS, "0000000000000000")
{
}

BatteryLimits::BatteryLimits(float charge_voltage,
                             float discharge_voltage,
                             float charge_current,
                             float discharge_current):
   StandardDataFrame(ID_BATTERY_LIMITS, "0000000000000000")
{
   setChargeVoltage(charge_voltage);
   setDischargeVoltage(discharge_voltage);
   setChargeCurrent(charge_current);
   setDischargeCurrent(discharge_current);
}

BatteryLimits& BatteryLimits::setChargeVoltage(float voltage)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(voltage, 10), LSB_FIRST);
   return *this;
}

BatteryLimits& BatteryLimits::setDischargeVoltage(float voltage)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(voltage, 10), LSB_FIRST);
   return *this;
}

BatteryLimits& BatteryLimits::setChargeCurrent(float current)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(current + 3000, 10), LSB_FIRST);
   return *this;
}

BatteryLimits& BatteryLimits::setDischargeCurrent(float current)
{
   setUnsignedShort(6, limitScaledToUnsignedShort(current + 3000, 10), LSB_FIRST);
   return *this;
}

}
}
}
