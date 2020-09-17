/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryState.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

BatteryState::BatteryState():
   StandardDataFrame(ID_BATTERY_STATE, "0000000000000000")
{
}
   
BatteryState::BatteryState(float soc_percent,
                           float soh_percent,
                           float energy_remaining_kwh,
                           float full_charged_energy_kwh):
   StandardDataFrame(ID_BATTERY_STATE, "0000000000000000")
{
   setSocPercent(soc_percent);
   setSohPercent(soh_percent);
   setEnergyRemainingKwh(energy_remaining_kwh);
   setFullChargedEnergyKwh(full_charged_energy_kwh);
}
   
BatteryState& BatteryState::setSocPercent(float soc)
{
   setUnsignedShort(0, soc*100);
   return *this;
}
   
BatteryState& BatteryState::setSohPercent(float soh)
{
   setUnsignedShort(2, soh*100);
   return *this;
}
   
BatteryState& BatteryState::setEnergyRemainingKwh(float energy)
{
   setUnsignedShort(4, energy*100);
   return *this;
}
   
BatteryState& BatteryState::setFullChargedEnergyKwh(float energy)
{
   setUnsignedShort(6, energy*100);
   return *this;
}


}
}
}
