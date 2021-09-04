/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "InverterIdentity.hpp"
#include "Ids.hpp"
#include "logging/Hex.hpp"
#include <math.h>

namespace can {
namespace messages {
namespace SMA {

InverterIdentity::InverterIdentity(const DataFrame& frame):
      Message(ID_INVERTER_IDENTITY),
      m_software_version(),
      m_inverter_id(),
      m_configuration()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   m_software_version = frame.getUnsignedLong(0, 4);
   m_inverter_id = frame.getUnsignedShort(4);
   m_configuration = frame.getByte(6);
   
   setValid();
}

InverterIdentity::InverterIdentity():
      Message(ID_INVERTER_IDENTITY),
      m_software_version(),
      m_inverter_id(),
      m_configuration()
{
}

uint32_t InverterIdentity::getSoftwareVersion() const
{
   return m_software_version;
}

uint16_t InverterIdentity::getInverterId() const
{
   return m_inverter_id;
}

uint8_t  InverterIdentity::getConfiguration() const
{
   return m_configuration;
}

void InverterIdentity::toStream(logging::ostream& os) const
{
   os << "InverterIdentity: ";

   if (not valid())
   {
      os << "invalid";
   }
   else
   {
      os << "SoftwareVersion=" << getSoftwareVersion()
         << " InverterId=" << getInverterId()
         << " Configuration=0x" << logging::HexByte(getConfiguration());
   }
}

}
}
}

