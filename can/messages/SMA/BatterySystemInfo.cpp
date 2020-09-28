/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatterySystemInfo.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SMA {
namespace {

const float MAX_VALUE = 6553.5;
const float SCALE = 10;

uint16_t limitAndScale(float v)
{
   if (v < 0) return 0;
   if (v > MAX_VALUE) return 0xFFFF;
   return v*SCALE;
}

}

BatterySystemInfo::BatterySystemInfo():
   StandardDataFrame(ID_BATTERY_SYSTEM_INFO, "0000000000000000")
{
}

BatterySystemInfo::BatterySystemInfo(unsigned version,
                                     float capacity_kwh,
                                     uint8_t number_of_modules,
                                     uint8_t manufacturer_id):
      StandardDataFrame(ID_BATTERY_SYSTEM_INFO, "0000000000000000")
{
   setVersion(version);
   setCapacityKwh(capacity_kwh);
   setNumberOfModules(number_of_modules);
   setManufacturerId(manufacturer_id);
}

BatterySystemInfo& BatterySystemInfo::setVersion(unsigned version)
{
   setUnsignedLong(0, version);
   return *this;
}
BatterySystemInfo& BatterySystemInfo::setCapacityKwh(float capacity_kwh)
{
   setUnsignedShort(4, limitAndScale(capacity_kwh));
   return *this;
}

BatterySystemInfo& BatterySystemInfo::setNumberOfModules(uint8_t number_of_modules)
{
   setByte(6, number_of_modules);
   return *this;
}

BatterySystemInfo& BatterySystemInfo::setManufacturerId(uint8_t manufacturer_id)
{
   setByte(7, manufacturer_id);
   return *this;
}


}
}
}
