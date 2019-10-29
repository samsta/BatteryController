#include "PackTemperatures.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {
namespace {

}

PackTemperatures::PackTemperatures(const DataFrame& f): m_valid(false)
{
   if (f.id() != 0x7bb) return;
   if (f.size() != 16) return;
   if (f.data()[1] != 4) return;

   for (unsigned k = 0; k < NUM_PACKS; k++)
   {
      m_temperatures[k] = int8_t(f.data()[3*k + 4]);
   }

   m_valid = true;
}

bool PackTemperatures::valid() const
{
   return m_valid;
}

float PackTemperatures::getTemperature(unsigned pack_index) const
{
   if (pack_index >= NUM_PACKS) return NAN;
   return m_temperatures[pack_index];
}

logging::ostream& operator<<(logging::ostream& os, const PackTemperatures& temperatures)
{
   os << "PackTemperatures: ";

   if (not temperatures.valid()) return os << "invalid";

   for (unsigned k = 0; k < PackTemperatures::NUM_PACKS; k++)
   {
      if (k != 0)
      {
         os << ", ";
      }
      os << temperatures.getTemperature(k) << "degC";
   }
   return os;
}


}
}
}
