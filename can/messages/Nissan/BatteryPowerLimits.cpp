/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryPowerLimits.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {
namespace {

}

BatteryPowerLimits::BatteryPowerLimits(const DataFrame& f):
   Message(ID_LBC_POWER_LIMITS, GROUP_NONE),
   m_discharge_power_limit_kw(),
   m_charge_power_limit_kw()
{
   if (f.id() != id()) return;
   if (f.size() != 8) return;

   uint32_t bits10;

   bits10 = f.getByte(0) << 2;
   bits10 |= f.getByte(1) >> 6;

   if (bits10 == 0x3FF) return; // default/invalid
   // scaling is 0.25
   m_discharge_power_limit_kw = 0.25 * (float)bits10;

   bits10 = (f.getByte(1) & 0x3F) << 4;
   bits10 |= f.getByte(2) >> 4;
   if (bits10 == 0x3FF) return; // default/invalid
   m_charge_power_limit_kw = 0.25 * (float)bits10;

   setValid();
}

BatteryPowerLimits::BatteryPowerLimits():
   Message(ID_LBC_POWER_LIMITS, GROUP_NONE),
   m_discharge_power_limit_kw(),
   m_charge_power_limit_kw()
{
}

float BatteryPowerLimits::getDischargePowerLimit_kW() const
{
   return m_discharge_power_limit_kw;
}

float BatteryPowerLimits::getChargePowerLimit_kW() const
{
   return m_charge_power_limit_kw;
}

BatteryPowerLimits& BatteryPowerLimits::setDischargePowerLimit_kW(float power)
{
   m_discharge_power_limit_kw = power;
   setValid();
   return *this;
}

BatteryPowerLimits& BatteryPowerLimits::setChargePowerLimit_kW(float power)
{
   m_charge_power_limit_kw = power;
   setValid();
   return *this;
}

void BatteryPowerLimits::toStream(logging::ostream& os) const
{
   os << "BatteryPowerLimits: ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "Discharge Limit=" << m_discharge_power_limit_kw << "kW "
      << "Charge Limit=" << m_charge_power_limit_kw << "kW ";
}


}
}
}




