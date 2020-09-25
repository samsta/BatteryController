/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYLIMITS_HPP
#define _CAN_MESSAGES_SMA_BATTERYLIMITS_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatteryLimits: public StandardDataFrame
{
public:
   BatteryLimits();
   BatteryLimits(float charge_voltage,
                 float discharge_voltage,
                 float charge_current,
                 float discharge_current);
   
   BatteryLimits& setChargeVoltage(float voltage);
   BatteryLimits& setDischargeVoltage(float voltage);
   BatteryLimits& setChargeCurrent(float current);
   BatteryLimits& setDischargeCurrent(float current);
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYLIMITS_HPP
