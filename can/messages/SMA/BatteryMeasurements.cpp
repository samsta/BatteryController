/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryMeasurements.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SMA {

BatteryMeasurements::BatteryMeasurements():
   StandardDataFrame(ID_BATTERY_MEASUREMENTS, "0000000000000000")
{
}
   
BatteryMeasurements::BatteryMeasurements(float voltage,
                                         float current,
                                         float temperature,
                                         State state,
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
   setUnsignedShort(0, limitScaledToUnsignedShort(voltage, 10));
   return *this;
}

BatteryMeasurements& BatteryMeasurements::setCurrent(float current)
{
   setUnsignedShort(2, limitScaledToSignedShort(current, 10));
   return *this;
}

BatteryMeasurements& BatteryMeasurements::setTemperature(float temperature)
{
   setUnsignedShort(4, limitScaledToSignedShort(temperature, 10));
   return *this;
}

BatteryMeasurements& BatteryMeasurements::setState(State state)
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
