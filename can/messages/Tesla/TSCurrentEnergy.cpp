/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "TSCurrentEnergy.hpp"
#include "logging/Hex.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Tesla {
namespace {

}

TSCurrentEnergy::TSCurrentEnergy():
   Message(ID_TS_CUR_ENERGY, GROUP_NONE),
   m_charge_current_limit(),
   m_discharge_current_limit(),
   m_stored_energy(),
   m_battery_capacity()
{
}

TSCurrentEnergy::TSCurrentEnergy(const DataFrame& frame):
   Message(ID_TS_CUR_ENERGY, GROUP_NONE),
   m_charge_current_limit(),
   m_discharge_current_limit(),
   m_stored_energy(),
   m_battery_capacity()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   m_stored_energy = frame.getUnsignedShort(0) / 64.0f;
   m_battery_capacity = frame.getUnsignedShort(2) / 64.0f;
   m_charge_current_limit = frame.getUnsignedShort(4) / 64.0f;
   m_discharge_current_limit = frame.getUnsignedShort(6) / 64.0f;
   
   setValid();
}


void TSCurrentEnergy::toStream(logging::ostream& os) const
{
   os << "TSCurrentEnergy: 0x" << logging::Hex(ID_TS_CUR_ENERGY) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "ChCurLmt= " << m_charge_current_limit << " A "
      << "DisCurLmt= " << m_discharge_current_limit << " A "
      << "StorEngy= " << m_stored_energy << " kWh "
      << "BatCap= " << m_battery_capacity << " kWh";
}


}
}
}




