/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryLimits.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SMA {
namespace {

const float MAX_VALUE = 6553.5;
const unsigned SCALE = 10;

uint16_t limitAndScale(float v)
{
   if (v > MAX_VALUE) return 0xFFFF;
   return max(v, 0.f)*SCALE;
}
   
}

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
   setUnsignedShort(0, limitAndScale(voltage));
   return *this;
}
   
BatteryLimits& BatteryLimits::setDischargeVoltage(float voltage)
{
   setUnsignedShort(2, limitAndScale(voltage));
   return *this;
}
   
BatteryLimits& BatteryLimits::setChargeCurrent(float current)
{
   setUnsignedShort(6, limitAndScale(current));
   return *this;
}
   
BatteryLimits& BatteryLimits::setDischargeCurrent(float current)
{
   setUnsignedShort(4, limitAndScale(current));
   return *this;
}

}
}
}
