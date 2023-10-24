/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryStatus.hpp"
#include "can/messages/SINEX/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SINEX {

BatteryStatus::BatteryStatus():
   StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000"),
   m_battery_status(0)
{
   m_alarms[BatteryStatus::AlarmLevel::MILD] = 0;
   m_alarms[BatteryStatus::AlarmLevel::MODERATE] = 0;
   m_alarms[BatteryStatus::AlarmLevel::SEVERE] = 0;
}

// BatteryStatus::BatteryStatus(BatteryStatusFlag battery_status1, BatteryStatusFlag battery_status2 = BatteryStatus::BatteryStatusFlag::BSF_CLEAR):
BatteryStatus::BatteryStatus(BatteryStatusFlag battery_status1):
   StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000"),
   m_battery_status(0)
{
   m_alarms[BatteryStatus::AlarmLevel::MILD] = 0;
   m_alarms[BatteryStatus::AlarmLevel::MODERATE] = 0;
   m_alarms[BatteryStatus::AlarmLevel::SEVERE] = 0;
   setBatteryStatus(battery_status1);
   // setBatteryStatus(battery_status2);
}

// BatteryStatus& BatteryStatus::setBatteryStatus(BatteryStatusFlag battery_status)
// {
//    setUnsignedShort(0, battery_status, LSB_FIRST);
//    return *this;
// }

void BatteryStatus::setBatteryStatus(BatteryStatusFlag battery_status)
{
   m_battery_status |= battery_status;
   setUnsignedShort(0, m_battery_status, LSB_FIRST);
}

void BatteryStatus::clearBatteryStatus(BatteryStatusFlag battery_status)
{
   m_battery_status &= ~battery_status;
   setUnsignedShort(0, m_battery_status, LSB_FIRST);
}

bool BatteryStatus::hasAlarm(AlarmLevel level) const
{
  return m_alarms[level] != 0;
}

bool BatteryStatus::hasAlarm(AlarmLevel level, AlarmFlag alarm) const
{
   return m_alarms[level] & alarm;
}

void BatteryStatus::setAlarm(AlarmLevel level, AlarmFlag alarm)
{
   m_alarms[level] |= alarm;
   setUnsignedShort(level, m_alarms[level], LSB_FIRST);
}

void BatteryStatus::clearAlarm(AlarmLevel level, AlarmFlag alarm)
{
   m_alarms[level] &= ~alarm;
   setUnsignedShort(level, m_alarms[level], LSB_FIRST);
}

}
}
}
