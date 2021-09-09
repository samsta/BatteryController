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

   enum Alarm {
      MOD_HIGH_VOLTAGE = 0x400,
      MOD_LOW_VOLTAGE = 0x300,
      DISCHARGE_OVER_CURRENT = 0x200,
      CHARGE_OVER_CURRENT = 0x100,
      DISCHARGE_CELL_HIGH_TEMP = 0x80,
      DISCHARGE_CELL_LOW_TEMP = 0x40,
      CHARGE_CELL_HIGH_TEMP = 0x20,
      CHARGE_CELL_LOW_TEMP = 0x10,
      CHARGE_SYSTEM_HIGH_VOLTAGE = 0x08,
      DISCHARGE_SYSTEM_LOW_VOLTAGE = 0x04,
      SINGLE_CELL_HIGH_VOLTAGE = 0x02,
      SINGLE_CELL_LOW_VOLTAGE = 0x01,
      NO_ALARM = 0
   };

//   inline Alarm operator|(Alarm a, Alarm b)
//   {
//       return static_cast<Alarm>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
//   }


   BatteryStatus();
   BatteryStatus(BasicStatus basic_status,
               //unsigned cycle_period,
               //unsigned error,
               Alarm alarm);
               //unsigned protection);
   BatteryStatus& setBasicStatus(BasicStatus basic_status);
   BatteryStatus& setAlarm(Alarm alarm);
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYSTATUS_HPP_ */
