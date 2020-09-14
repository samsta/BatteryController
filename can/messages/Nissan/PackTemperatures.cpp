/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "PackTemperatures.hpp"
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

PackTemperatures::PackTemperatures(const DataFrame& f): m_valid(false)
{
   if (f.id() != ID_LBC_DATA_REPLY) return;
   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != GROUP_PACK_TEMPERATURES) return;

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

   m_valid = true;
}

PackTemperatures::PackTemperatures(): m_valid(false)
{
}

bool PackTemperatures::valid() const
{
   return m_valid;
}

float PackTemperatures::getTemperature(unsigned pack_index) const
{
   if (pack_index >= NUM_SENSORS) return NAN;
   return m_temperatures[pack_index];
}

PackTemperatures& PackTemperatures::setTemperature(unsigned pack_index, float temperature)
{
   if (pack_index >= NUM_SENSORS) return *this;
   m_valid = true;
   m_temperatures[pack_index] = temperature;
   return *this;
}

logging::ostream& operator<<(logging::ostream& os, const PackTemperatures& temperatures)
{
   os << "PackTemperatures: ";

   if (not temperatures.valid()) return os << "invalid";

   for (unsigned k = 0; k < PackTemperatures::NUM_SENSORS; k++)
   {
      if (k != 0)
      {
         os << ", ";
      }
      if (isnan(temperatures.getTemperature(k)))
      {
         os << "---";
      }
      else
      {
         os << temperatures.getTemperature(k) << "degC";
      }
   }
   return os;
}


}
}
}
