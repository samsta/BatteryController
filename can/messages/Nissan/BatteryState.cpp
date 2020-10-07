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
   Message(GROUP_BATTERY_STATE),
   m_pack_voltage(),
   m_health_pc(),
   m_capacity_ah(),
   m_soc_pc()
{
   if (f.id() != ID_LBC_DATA_REPLY) return;
   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != dataGroup()) return;

   m_pack_voltage = 0.01 * f.getUnsignedShort(20);
   m_health_pc = 0.01 * f.getUnsignedShort(28);
   m_capacity_ah = 0.0001 * f.getUnsignedLong(35, 3);
   m_soc_pc = 0.0001 * f.getUnsignedLong(31, 3);

   setValid();
}

float BatteryState::getPackVoltage() const
{
   return m_pack_voltage;
}

float BatteryState::getHealthPercent() const
{
   return m_health_pc;
}

float BatteryState::getCapacityAh() const
{
   return m_capacity_ah;
}

float BatteryState::getSocPercent() const
{
   return m_soc_pc;
}

logging::ostream& operator<<(logging::ostream& os, const BatteryState& state)
{
   os << "BatteryState: ";

   if (not state.valid()) return os << "invalid";

   os << "Voltage=" << state.getPackVoltage() << "V "
      << "Health=" << state.getHealthPercent() << "% "
      << "SOC=" << state.getSocPercent() << "% "
      << "Capacity=" << state.getCapacityAh() << "Ah";
   return os;
}


}
}
}
