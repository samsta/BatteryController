/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "BatteryStatus.hpp"
#include "logging/Hex.hpp"

namespace can {
namespace messages {
namespace Nissan {

BatteryStatus::BatteryStatus():
      m_voltage(),
      m_current(),
      m_security_byte(),
      m_multiplex_byte(),
      m_valid(false)
{

}

BatteryStatus::BatteryStatus(const DataFrame& frame):
      m_voltage(),
      m_current(),
      m_security_byte(),
      m_multiplex_byte(),
      m_valid(false)
{
   if (frame.id() != ID_BATTERY_STATUS) return;
   if (frame.size() != 8) return;

   // TODO: verify that current and voltage are correct!
   int16_t raw_current = (int16_t(frame.getByte(0)) << 3) | (frame.getByte(1) >> 5);
   if (raw_current & 0x0400) raw_current |= 0xf800;

   uint16_t raw_voltage = (int16_t(frame.getByte(2)) << 2) | (frame.getByte(3) >> 6);
   
   m_current = raw_current * 0.5;
   m_voltage = raw_voltage * 0.5;
   m_security_byte = frame.getByte(7);
   m_multiplex_byte = frame.getByte(6);
   
   m_valid = true;
}
   
float BatteryStatus::getVoltage() const
{
   return m_voltage;
}

float BatteryStatus::getCurrent() const
{
   return m_current;
}

uint8_t BatteryStatus::getSecurityByte() const
{
   return m_security_byte;
}

uint8_t BatteryStatus::getMultiplexByte() const
{
   return m_multiplex_byte;
}

bool BatteryStatus::valid() const
{
   return m_valid;
}

logging::ostream& operator<<(logging::ostream& os, const BatteryStatus& status)
{
   os << "BatteryStatus: ";
   if (status.valid())
   {
      return os << "Current=" << status.getCurrent()
                << "A Voltage=" << status.getVoltage()
                << "V SecurityByte=0x" << logging::HexByte(status.getSecurityByte())
                << " MultiplexByte=0x" << logging::HexByte(status.getMultiplexByte());
   }
   else
   {
      return os << "invalid";
   }
}

}
}
}
