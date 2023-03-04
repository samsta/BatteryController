/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_SINEX_BATTERYSTATUS_HPP_
#define CAN_MESSAGES_SINEX_BATTERYSTATUS_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/SINEX/Ids.hpp"

namespace can {
namespace messages {
namespace SINEX {

class BatteryStatus: public StandardDataFrame
{
public:
   enum BatteryStatusFlag {
      BSF_CLEAR = 0,
      BSF_STANDBY_0 = 1u << 0,
      BSF_STANDBY_1 = 1u << 1,
      BSF_STANDBY_2 = 1u << 2,
      BSF_FORCED_CHARGING = 1u << 3,
      BSF_BATTERY_EMPTY = 1u << 4,
      BSF_BATTERY_FULL = 1u << 5,
      BSF_DISCHARGE_CONTACTOR_CLOSED = 1u << 6,
      BSF_CHARGING_CONTACTOR_CLOSED = 1u << 7,
      BSF_STANDBY_8 = 1u << 8,
      BSF_ALTERNATE_STATE_9 = 1u << 9,
      BSF_STANDBY_10 = 1u << 10,
      BSF_STANDBY_11 = 1u << 11,
      BSF_STANDBY_12 = 1u << 12,
      BSF_STANDBY_13 = 1u << 13,
      BSF_STANDBY_14 = 1u << 14,
      BSF_STANDBY_15 = 1u << 15 
   };

   enum AlarmFlag {
      ALTERNATE_ALM_0 = 1u << 0,
      ALTERNATE_ALM_1 = 1u << 1,
      ALTERNATE_ALM_2 = 1u << 2,
      EXCESSIVE_TEMP_DIFF = 1u << 3,
      EXCESSIVE_VOLT_DIFF = 1u << 4,
      INSULATION_FAIL = 1u << 5,
      CHARGING_OVERCURRENT = 1u << 6,
      DISCHARGE_OVERCURRENT = 1u << 7,
      MONOMER_SOC_TOO_HIGH = 1u << 8,
      MONOMER_SOC_TOO_LOW = 1u << 9,
      MONOMER_OVER_TEMP = 1u << 10,
      MONOMER_UNDER_TEMP = 1u << 11,
      TOTAL_PRESSURE_HIGH = 1u << 12,
      TOTAL_PRESSURE_LOW = 1u << 13,
      SINGLE_OVER_PRESSURE = 1u << 14,
      SINGLE_UNDER_VOLTAGE = 1u << 15
   };

   enum AlarmLevel { // start byte in CAN msg
      MILD = 3,
      MODERATE = 5,
      SEVERE = 7
   };

   BatteryStatus();
   // BatteryStatus(BatteryStatusFlag basic_status1, BatteryStatusFlag basic_status2 = BatteryStatus::BatteryStatusFlag::BSF_CLEAR);
   BatteryStatus(BatteryStatusFlag basic_status1);
   // BatteryStatus& setBatteryStatus(BatteryStatusFlag battery_status);
   void setBatteryStatus(BatteryStatusFlag battery_status);
   void clearBatteryStatus(BatteryStatusFlag battery_status);
   BatteryStatusFlag getBatteryStatus() const;

   bool hasAlarm(AlarmLevel level) const;
   bool hasAlarm(AlarmLevel level, AlarmFlag flag) const;
   void setAlarm(AlarmLevel level, AlarmFlag alarm);
   void clearAlarm(AlarmLevel level, AlarmFlag alarm);

   uint16_t m_battery_status;
   uint16_t m_alarms[8];
};

}
}
}

#endif /* CAN_MESSAGES_SINEX_BATTERYSTATUS_HPP_ */
