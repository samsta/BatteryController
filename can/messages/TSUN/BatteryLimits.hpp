/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYLIMITS_HPP_
#define CAN_MESSAGES_TSUN_BATTERYLIMITS_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

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

#endif /* CAN_MESSAGES_TSUN_BATTERYLIMITS_HPP_ */
