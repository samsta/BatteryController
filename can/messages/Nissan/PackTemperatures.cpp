/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "PackTemperatures.hpp"
#include "logging/Hex.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {
namespace {

const unsigned GROUP_SIZE = 16;
struct __attribute__((__packed__)) Temperature
{
   uint16_t raw_adc;
   int8_t   degc;
};

}

PackTemperatures::PackTemperatures(const DataFrame& f): Message(ID_LBC_DATA_REPLY, GROUP_PACK_TEMPERATURES)
{
   if (f.id() != id()) return;
   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != dataGroup()) return;

   const Temperature* temperatures(reinterpret_cast<const Temperature*>(&f.data()[2]));

   for (unsigned k = 0; k < NUM_SENSORS; k++)
   {
      if (temperatures[k].raw_adc == 0xFFFF)
      {
         // likely no sensor present
         m_temperatures[k] = NAN;
      }
      else
      {
         m_temperatures[k] = temperatures[k].degc;
      }
   }

   setValid();
}

PackTemperatures::PackTemperatures(): Message(ID_LBC_DATA_REPLY, GROUP_PACK_TEMPERATURES)
{
}

float PackTemperatures::getTemperature(unsigned pack_index) const
{
   if (pack_index >= NUM_SENSORS) return NAN;
   return m_temperatures[pack_index];
}

PackTemperatures& PackTemperatures::setTemperature(unsigned pack_index, float temperature)
{
   if (pack_index >= NUM_SENSORS) return *this;
   setValid();
   m_temperatures[pack_index] = temperature;
   return *this;
}

void PackTemperatures::toStream(logging::ostream& os) const
{
   os << "PackTemperatures: " << logging::Hex(ID_LBC_DATA_REPLY) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }
   
   for (unsigned k = 0; k < PackTemperatures::NUM_SENSORS; k++)
   {
      if (k != 0)
      {
         os << ", ";
      }
      if (isnan(getTemperature(k)))
      {
         os << "---";
      }
      else
      {
         os << getTemperature(k) << "degC";
      }
   }
}


}
}
}
