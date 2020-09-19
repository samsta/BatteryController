/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryState.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace SMA {
namespace {

const float MAX_PERCENT = 100;
const float MAX_KWH     = 655.35;

}

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
   soc = clamp(soc, 0.f, MAX_PERCENT);
   setUnsignedShort(0, soc*100);
   return *this;
}
   
BatteryState& BatteryState::setSohPercent(float soh)
{
   soh = clamp(soh, 0.f, MAX_PERCENT);
   setUnsignedShort(2, soh*100);
   return *this;
}
   
BatteryState& BatteryState::setEnergyRemainingKwh(float energy)
{
   energy = max(energy, 0.f);
   setUnsignedShort(4, energy < MAX_KWH ? energy*100 : 0xFFFF);
   return *this;
}
   
BatteryState& BatteryState::setFullChargedEnergyKwh(float energy)
{
   energy = max(energy, 0.f);
   setUnsignedShort(6, energy < MAX_KWH ? energy*100 : 0xFFFF);
   return *this;
}


}
}
}
