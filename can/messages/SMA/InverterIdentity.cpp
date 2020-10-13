/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "InverterIdentity.hpp"
#include "Ids.hpp"
#include "logging/Hex.hpp"
#include <math.h>

namespace can {
namespace messages {
namespace SMA {

InverterIdentity::InverterIdentity(const DataFrame& frame):
      m_software_version(),
      m_inverter_id(),
      m_configuration(),
      m_valid(false)
{
   if (frame.id() != ID_INVERTER_IDENTITY) return;
   if (frame.size() != 8) return;

   m_software_version = frame.getUnsignedLong(0, 4);
   m_inverter_id = frame.getUnsignedShort(4);
   m_configuration = frame.getByte(6);
   
   m_valid = true;
}

InverterIdentity::InverterIdentity():
      m_software_version(),
      m_inverter_id(),
      m_configuration(),
      m_valid(false)
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

bool InverterIdentity::valid() const
{
   return m_valid;
}

logging::ostream& operator<<(logging::ostream& os, const InverterIdentity& identity)
{
   os << "InverterIdentity: ";

   if (not identity.valid()) return os << "invalid";

   os << "SoftwareVersion=" << identity.getSoftwareVersion()
      << " InverterId=" << identity.getInverterId()
      << " Configuration=0x" << logging::HexByte(identity.getConfiguration());
   return os;
}

}
}
}

