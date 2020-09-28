/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryMeasurements.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SMA {
namespace {

const unsigned SCALE = 10;
const float MAX_UNSIGNED = 6553.5;
const float MIN_SIGNED = -3276.8;
const float MAX_SIGNED = 3276.7;

uint16_t limitAndScaleUnsigned(float v)
{
   if (v < 0) return 0;
   if (v > MAX_UNSIGNED) return 0xFFFF;
   return v*SCALE;
}

int16_t limitAndScaleSigned(float v)
{
   if (v > MAX_SIGNED) return 0x7FFF;
   if (v < MIN_SIGNED) return 0x8000;
   return v*SCALE;
}
   
}

BatteryMeasurements::BatteryMeasurements():
   StandardDataFrame(ID_BATTERY_MEASUREMENTS, "0000000000000000")
{
}
   
BatteryMeasurements::BatteryMeasurements(float voltage,
                                         float current,
                                         float temperature,
                                         unsigned state,
                                         unsigned inverter_control_flags):
   StandardDataFrame(ID_BATTERY_MEASUREMENTS, "0000000000000000")
{
   setVoltage(voltage);
   setCurrent(current);
   setTemperature(temperature);
   setState(state);
   setInverterControlFlags(inverter_control_flags);
}

BatteryMeasurements& BatteryMeasurements::setVoltage(float voltage)
{
   setUnsignedShort(0, limitAndScaleUnsigned(voltage));
   return *this;
}

BatteryMeasurements& BatteryMeasurements::setCurrent(float current)
{
   setUnsignedShort(2, limitAndScaleSigned(current));
   return *this;
}

BatteryMeasurements& BatteryMeasurements::setTemperature(float temperature)
{
   setUnsignedShort(4, limitAndScaleSigned(temperature));
   return *this;
}

BatteryMeasurements& BatteryMeasurements::setState(unsigned state)
{
   setByte(6, state);
   return *this;
}

BatteryMeasurements& BatteryMeasurements::setInverterControlFlags(unsigned inverter_control_flags)
{
   setByte(7, inverter_control_flags);
   return *this;
}


}
}
}
