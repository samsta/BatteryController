/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatterySystemInfo.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SMA {

BatterySystemInfo::BatterySystemInfo():
   StandardDataFrame(ID_BATTERY_SYSTEM_INFO, "0000000000000000")
{
}

BatterySystemInfo::BatterySystemInfo(uint32_t version,
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

BatterySystemInfo& BatterySystemInfo::setVersion(uint32_t version)
{
   setUnsignedLong(0, version);
   return *this;
}
BatterySystemInfo& BatterySystemInfo::setCapacityKwh(float capacity_kwh)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(capacity_kwh, 10));
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
