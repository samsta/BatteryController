/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryState.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {
namespace {

const unsigned GROUP_SIZE = 41;

}

BatteryState::BatteryState(const DataFrame& f):
   Message(ID_LBC_DATA_REPLY, GROUP_BATTERY_STATE),
   m_pack_voltage(),
   m_health_pc(),
   m_capacity_ah(),
   m_soc_pc()
{
   if (f.id() != id()) return;
   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != dataGroup()) return;

   m_pack_voltage = 0.01 * f.getUnsignedShort(20);
   m_health_pc = 0.01 * f.getUnsignedShort(28);
   m_capacity_ah = 0.0001 * f.getUnsignedLong(35, 3);
   m_soc_pc = 0.0001 * f.getUnsignedLong(31, 3);

   setValid();
}

BatteryState::BatteryState():
   Message(ID_LBC_DATA_REPLY, GROUP_BATTERY_STATE)
{
}

float BatteryState::getPackVoltage() const
{
   return m_pack_voltage;
}

float BatteryState::getHealthPercent() const
{
   return m_health_pc;
}

BatteryState& BatteryState::setHealthPercent(float health)
{
   m_health_pc = health;
   setValid();
   return *this;
}

float BatteryState::getCapacityAh() const
{
   return m_capacity_ah;
}

float BatteryState::getSocPercent() const
{
   return m_soc_pc;
}

BatteryState& BatteryState::setSocPercent(float soc)
{
   m_soc_pc = soc;
   setValid();
   return *this;
}

void BatteryState::toStream(logging::ostream& os) const
{
   os << "BatteryState: ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "Voltage=" << getPackVoltage() << "V "
      << "Health=" << getHealthPercent() << "% "
      << "SOC=" << getSocPercent() << "% "
      << "Capacity=" << getCapacityAh() << "Ah";
}


}
}
}
