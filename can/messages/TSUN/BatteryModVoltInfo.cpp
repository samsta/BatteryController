/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryModVoltInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryModVoltInfo::BatteryModVoltInfo():
   StandardDataFrame(ID_BATTERY_MODULE_VOLT_INFO, "0000000000000000")
{
}

BatteryModVoltInfo::BatteryModVoltInfo(float max_single_module_voltage,
                       float min_single_module_voltage,
                       float max_module_voltage_number,
                       float min_module_voltage_number):
   StandardDataFrame(ID_BATTERY_MODULE_VOLT_INFO, "0000000000000000")
{
   setMaxSingleModuleVoltage(max_single_module_voltage);
   setMinSingleModuleVoltage(min_single_module_voltage);
   setMaxModuleVoltageNumber(max_module_voltage_number);
   setMinModuleVoltageNumber(min_module_voltage_number);
}

BatteryModVoltInfo& BatteryModVoltInfo::setMaxSingleModuleVoltage(float max_single_module_voltage)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(max_single_module_voltage, 1000), LSB_FIRST);
   return *this;
}

BatteryModVoltInfo& BatteryModVoltInfo::setMinSingleModuleVoltage(float min_single_module_voltage)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(min_single_module_voltage, 1000), LSB_FIRST);
   return *this;
}

BatteryModVoltInfo& BatteryModVoltInfo::setMaxModuleVoltageNumber(float max_module_voltage_number)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(max_module_voltage_number, 1), LSB_FIRST);
   return *this;
}

BatteryModVoltInfo& BatteryModVoltInfo::setMinModuleVoltageNumber(float min_module_voltage_number)
{
   setUnsignedShort(6, limitScaledToUnsignedShort(min_module_voltage_number, 1), LSB_FIRST);
   return *this;
}

}
}
}
