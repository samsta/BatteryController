/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryVoltsCurrent.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {
namespace {

}

BatteryVoltsCurrent::BatteryVoltsCurrent(const DataFrame& f):
   Message(ID_LBC_VOLTS_CURRENT),
   m_total_voltage(),
   m_total_current()
{
   if (f.id() != id()) return;
   if (f.size() != 8) return;

   uint32_t bits10;
   int32_t bits11;

   bits11 = f.getByte(0) << 3;
   bits11 |= f.getByte(1) >> 5;

   // scaling is 0.5
   m_total_current = 0.5 * (float)bits11;

   bits10 = f.getByte(2) << 2;
   bits10 |= f.getByte(3) >> 6;
   if (bits10 == 0x3FF) return; // default/invalid
   m_total_voltage = 0.5 * (float)bits10;

   setValid();
}

BatteryVoltsCurrent::BatteryVoltsCurrent():
   Message(ID_LBC_VOLTS_CURRENT),
   m_total_voltage(),
   m_total_current()
{
}

float BatteryVoltsCurrent::getTotalVoltage() const
{
   return m_total_voltage;
}

float BatteryVoltsCurrent::getTotalCurrent() const
{
   return m_total_current;
}

void BatteryVoltsCurrent::toStream(logging::ostream& os) const
{
   os << "BatteryVoltsCurrent: ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "Total Volts=" << getTotalVoltage() << "V "
      << "Total Current=" << getTotalCurrent() << "A ";
}


}
}
}




