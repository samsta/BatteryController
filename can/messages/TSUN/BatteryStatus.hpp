/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYSTATUS_HPP_
#define CAN_MESSAGES_TSUN_BATTERYSTATUS_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryStatus: public StandardDataFrame
{
public:
   enum BasicStatus {
      BASIC_STATUS_SLEEP = 0,
      BASIC_STATUS_CHARGE = 1,
      BASIC_STATUS_DISCHARGE = 2,
      BASIC_STATUS_IDLE = 3,
   };

   enum Alarm { // bit number 0-11
      MOD_HIGH_VOLTAGE = 11,
      MOD_LOW_VOLTAGE = 10,
      DISCHARGE_OVER_CURRENT = 9,
      CHARGE_OVER_CURRENT = 8,
      DISCHARGE_CELL_HIGH_TEMP = 7,
      DISCHARGE_CELL_LOW_TEMP = 6,
      CHARGE_CELL_HIGH_TEMP = 5,
      CHARGE_CELL_LOW_TEMP = 4,
      CHARGE_SYSTEM_HIGH_VOLTAGE = 3,
      DISCHARGE_SYSTEM_LOW_VOLTAGE = 2,
      SINGLE_CELL_HIGH_VOLTAGE = 1,
      SINGLE_CELL_LOW_VOLTAGE = 0
   };

   BatteryStatus();
   BatteryStatus(BasicStatus basic_status);
   BatteryStatus& setBasicStatus(BasicStatus basic_status);
   BatteryStatus& setAlarms();

   bool hasAlarm() const;
   bool hasAlarm(Alarm alarm) const;
   void setAlarm(Alarm alarm);
   void clearAlarm(Alarm alarm);

   uint32_t m_alarms;
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYSTATUS_HPP_ */
