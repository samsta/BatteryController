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
   m_total_current(),
   m_usable_soc()
{
   if (f.id() != id()) return;
   if (f.size() != 8) return;

   uint32_t bits10;
   int16_t bits11;

   // put 11 bits in to a 16 bit signed number
   // then divide by 32 to get 11 bit value
   bits11 = f.getByte(0) << 8;
   bits11 |= f.getByte(1) & 0xE0;

   // scaling is 0.5
   m_total_current = 0.5 * (float)bits11/32.0;

   bits10 = f.getByte(2) << 2;
   bits10 |= f.getByte(3) >> 6;
   if (bits10 == 0x3FF) return; // default/invalid
   m_total_voltage = 0.5 * (float)bits10;

   // todo: check the value is less than 100, then what?
   m_usable_soc = f.getByte(4) & 0x7F;

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

unsigned BatteryVoltsCurrent::getUsableSOC() const
{
   return m_usable_soc;
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
	   << "Total Current=" << getTotalCurrent() << "A "
   	   << "Usable SOC=" << getUsableSOC() << "% ";
}


}
}
}




