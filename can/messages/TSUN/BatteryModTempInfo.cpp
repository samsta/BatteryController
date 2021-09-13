/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryModTempInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryModTempInfo::BatteryModTempInfo():
   StandardDataFrame(ID_BATTERY_MODULE_TEMP_INFO, "0000000000000000")
{
}

BatteryModTempInfo::BatteryModTempInfo(float max_single_module_temperature,
                       float min_single_module_temperature,
                       float max_module_temperature_number,
                       float min_module_temperature_number):
   StandardDataFrame(ID_BATTERY_MODULE_TEMP_INFO, "0000000000000000")
{
   setMaxSingleModuleTemp(max_single_module_temperature);
   setMinSingleModuleTemp(min_single_module_temperature);
   setMaxModuleTempNumber(max_module_temperature_number);
   setMinModuleTempNumber(min_module_temperature_number);
}

BatteryModTempInfo& BatteryModTempInfo::setMaxSingleModuleTemp(float max_single_module_temperature)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(max_single_module_temperature + 100, 10), LSB_FIRST);
   return *this;
}

BatteryModTempInfo& BatteryModTempInfo::setMinSingleModuleTemp(float min_single_module_temperature)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(min_single_module_temperature + 100, 10), LSB_FIRST);
   return *this;
}

BatteryModTempInfo& BatteryModTempInfo::setMaxModuleTempNumber(float max_module_temperature_number)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(max_module_temperature_number, 1), LSB_FIRST);
   return *this;
}

BatteryModTempInfo& BatteryModTempInfo::setMinModuleTempNumber(float min_module_temperature_number)
{
   setUnsignedShort(6, limitScaledToUnsignedShort(min_module_temperature_number, 1), LSB_FIRST);
   return *this;
}

}
}
}
