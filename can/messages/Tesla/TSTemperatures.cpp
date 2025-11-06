/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "TSTemperatures.hpp"
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

TSTemperatures::TSTemperatures():
   Message(ID_TS_TEMPS, GROUP_NONE),
   m_min_temperature(),
   m_max_temperature()
{
}

TSTemperatures::TSTemperatures(const DataFrame& frame):
   Message(ID_TS_TEMPS, GROUP_NONE),
   m_min_temperature(),
   m_max_temperature()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   m_max_temperature = frame.getSignedShort(0) / 100.0f;
   m_min_temperature = frame.getSignedShort(2) / 100.0f;
   m_average_temperature = frame.getSignedShort(4) / 100.0f;
   
   setValid();
}

float TSTemperatures::getMaxTemperature() const
{
   return m_max_temperature;
}

float TSTemperatures::getMinTempeature() const
{
   return m_min_temperature;
}

float TSTemperatures::getAvgTemperature() const
{
   return m_average_temperature;
}

void TSTemperatures::toStream(logging::ostream& os) const
{
   os << "TSTemperatures: 0x" << logging::Hex(ID_TS_TEMPS) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "MinTemp= " << m_min_temperature << " degC "
      << "MaxTemp= " << m_max_temperature << " degC";
}


}
}
}




