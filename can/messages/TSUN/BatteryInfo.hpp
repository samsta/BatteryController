/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYINFO_HPP_
#define CAN_MESSAGES_TSUN_BATTERYINFO_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryInfo: public StandardDataFrame
{
public:
   BatteryInfo();
   BatteryInfo(float pile_voltage,
               //float pile_current,
               //float bms_2nd_temp,
               unsigned soc,
               unsigned soh);
   BatteryInfo& setPileVoltage(float pile_voltage);
   //BatteryInfo& setPileCurrent(float pile_current);
   //BatteryInfo& setBMS2ndTemp(float bms_2nd_temp);
   BatteryInfo& setSOC(unsigned soc);
   BatteryInfo& setSOH(unsigned soh);
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYINFO_HPP_ */
