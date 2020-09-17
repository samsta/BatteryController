/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYSTATE_HPP
#define _CAN_MESSAGES_SMA_BATTERYSTATE_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatteryState: public StandardDataFrame
{
public:
   BatteryState();
   BatteryState(float soc_percent,
                float soh_percent,
                float energy_remaining_kwh,
                float full_charged_energy_kwh);
   
   BatteryState& setSocPercent(float soc);
   BatteryState& setSohPercent(float soc);
   BatteryState& setEnergyRemainingKwh(float energy);
   BatteryState& setFullChargedEnergyKwh(float energy);
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYSTATE_HPP
