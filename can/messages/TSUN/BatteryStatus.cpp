/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryStatus.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryStatus::BatteryStatus():
   StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000")
{
}

BatteryStatus::BatteryStatus(BasicStatus basic_status,
                              Alarm alarm):
   StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000")
{
   setBasicStatus(basic_status);
   setAlarm(alarm);
}

BatteryStatus& BatteryStatus::setBasicStatus(BasicStatus basic_status)
{
   setByte(0, basic_status);
   return *this;
}

BatteryStatus& BatteryStatus::setAlarm(Alarm alarm)
{
   setUnsignedShort(4, alarm, LSB_FIRST);
   return *this;
}

}
}
}
