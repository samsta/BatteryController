/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_SINEX_BATTERYLIMITSONE_HPP_
#define CAN_MESSAGES_SINEX_BATTERYLIMITSONE_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/SINEX/Ids.hpp"

namespace can {
namespace messages {
namespace SINEX {

class BatteryLimitsOne: public StandardDataFrame
{
public:
   BatteryLimitsOne();
   BatteryLimitsOne(float max_charge_current,
                     float max_discharge_current,
                     float total_voltage,
                     float total_current);

   BatteryLimitsOne& setMaxChargeCurrent(float current);
   BatteryLimitsOne& setMaxDischargeCurrent(float current);
   BatteryLimitsOne& setTotalVoltage(float voltage);
   BatteryLimitsOne& setTotalCurrent(float current);
};

}
}
}

#endif /* CAN_MESSAGES_SINEX_BATTERYLIMITSONE_HPP_ */
