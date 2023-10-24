/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_SINEX_BATTERYLIMITSTWO_HPP_
#define CAN_MESSAGES_SINEX_BATTERYLIMITSTWO_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/SINEX/Ids.hpp"

namespace can {
namespace messages {
namespace SINEX {

class BatteryLimitsTwo: public StandardDataFrame
{
public:
   BatteryLimitsTwo();
   BatteryLimitsTwo(float soc_percent,
                     float soh_percent,
                     float max_charge_voltage);

   BatteryLimitsTwo& setSOCPercent(float soc);
   BatteryLimitsTwo& setSOHPercent(float soh);
   BatteryLimitsTwo& setMaxChargingVoltage(float voltage);
};

}
}
}

#endif /* CAN_MESSAGES_SINEX_BATTERYLIMITSTWO_HPP_ */
