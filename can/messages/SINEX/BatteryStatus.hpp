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
      BSF_STANDBY_0 = 2^0,
      BSF_STANDBY_1 = 2^1,
      BSF_STANDBY_2 = 2^2,
      BSF_FORCED_CHARGING = 2^3,
      BSF_BATTERY_EMPTY = 2^4,
      BSF_BATTERY_FULL = 2^5,
      BSF_DISCHARGE_CONTACTOR_CLOSED = 2^6,
      BSF_CHARGING_CONTACTOR_CLOSED = 2^7,
      BSF_STANDBY_8 = 2^8,
      BSF_ALTERNATE_STATE_9 = 2^9,
      BSF_STANDBY_10 = 2^10,
      BSF_STANDBY_11 = 2^11,
      BSF_STANDBY_12 = 2^12,
      BSF_STANDBY_13 = 2^13,
      BSF_STANDBY_14 = 2^14,
      BSF_STANDBY_15 = 2^15 
   };

   enum AlarmFlag {
      ALTERNATE_ALM_0 = 2^0,
      ALTERNATE_ALM_1 = 2^1,
      ALTERNATE_ALM_2 = 2^2,
      EXCESSIVE_TEMP_DIFF = 2^3,
      EXCESSIVE_VOLT_DIFF = 2^4,
      INSULATION_FAIL = 2^5,
      CHARGING_OVERCURRENT = 2^6,
      DISCHARGE_OVERCURRENT = 2^7,
      MONOMER_SOC_TOO_HIGH = 2^8,
      MONOMER_SOC_TOO_LOW = 2^9,
      MONOMER_OVER_TEMP = 2^10,
      MONOMER_UNDER_TEMP = 2^11,
      TOTAL_PRESSURE_HIGH = 2^12,
      TOTAL_PRESSURE_LOW = 2^13,
      SINGLE_OVER_PRESSURE = 2^14,
      SINGLE_UNDER_VOLTAGE = 2^15
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

   bool hasAlarm() const;
   // bool hasAlarm(AlarmLevel level) const;
   // bool hasAlarm(AlarmLevel level, AlarmFlag alarm) const;
   // void setAlarm(AlarmLevel level, AlarmFlag alarm);
   // void clearAlarm(AlarmLevel level, AlarmFlag alarm);

   uint16_t m_battery_status;
   uint16_t m_alarms[8];
};

}
}
}

#endif /* CAN_MESSAGES_SINEX_BATTERYSTATUS_HPP_ */
