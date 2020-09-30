/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryIdentity.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SMA {

BatteryIdentity::BatteryIdentity():
   StandardDataFrame(ID_BATTERY_IDENTITY, "0000000000000000")
{
}

BatteryIdentity::BatteryIdentity(uint32_t serial_number,
                                 uint32_t manufacturing_date_unix_time):
      StandardDataFrame(ID_BATTERY_IDENTITY, "0000000000000000")
{
   setSerialNumber(serial_number);
   setManufacturingDateUnixTime(manufacturing_date_unix_time);
}

BatteryIdentity& BatteryIdentity::setSerialNumber(uint32_t serial_number)
{
   setUnsignedLong(0, serial_number);
   return *this;
}

BatteryIdentity& BatteryIdentity::setManufacturingDateUnixTime(uint32_t unix_time)
{
   setUnsignedLong(4, unix_time);
   return *this;
}

}
}
}
