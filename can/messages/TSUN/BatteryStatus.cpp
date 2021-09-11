/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryStatus.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryStatus::BatteryStatus():
   StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000"),
   m_alarms(0)
{
}

BatteryStatus::BatteryStatus(BasicStatus basic_status):
   StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000"),
   m_alarms(0)
{
   setBasicStatus(basic_status);
}

BatteryStatus& BatteryStatus::setBasicStatus(BasicStatus basic_status)
{
   setByte(0, basic_status);
   return *this;
}

BatteryStatus& BatteryStatus::setAlarms()
{
   setUnsignedShort(4, m_alarms, LSB_FIRST);
   return *this;
}

bool BatteryStatus::hasAlarm() const
{
   return m_alarms != 0;
}

bool BatteryStatus::hasAlarm(Alarm alarm) const
{
   return m_alarms & (1u << alarm);
}

void BatteryStatus::setAlarm(Alarm alarm)
{
   m_alarms |= 1u << alarm;
   setAlarms();
}

void BatteryStatus::clearAlarm(Alarm alarm)
{
   m_alarms &= ~(1u << alarm);
   setAlarms();
}


}
}
}
