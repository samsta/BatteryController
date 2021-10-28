/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "BatteryStatus.hpp"
#include "logging/Hex.hpp"

namespace can {
namespace messages {
namespace Nissan {

BatteryStatus::BatteryStatus():
      Message(ID_BATTERY_STATUS, GROUP_NONE),
      m_voltage(),
      m_current(),
      m_security_byte(),
      m_multiplex_byte()
{

}

BatteryStatus::BatteryStatus(const DataFrame& frame):
      Message(ID_BATTERY_STATUS, GROUP_NONE),
      m_voltage(),
      m_current(),
      m_security_byte(),
      m_multiplex_byte()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   // TODO: verify that current and voltage are correct!
   int16_t raw_current = (int16_t(frame.getByte(0)) << 3) | (frame.getByte(1) >> 5);
   if (raw_current & 0x0400) raw_current |= 0xf800;

   uint16_t raw_voltage = (int16_t(frame.getByte(2)) << 2) | (frame.getByte(3) >> 6);
   
   m_current = raw_current * 0.5;
   m_voltage = raw_voltage * 0.5;
   m_security_byte = frame.getByte(7);
   m_multiplex_byte = frame.getByte(6);
   
   m_usable_soc = frame.getByte(4) & 0x7F;

   setValid();
}
   
float BatteryStatus::getVoltage() const
{
   return m_voltage;
}

BatteryStatus& BatteryStatus::setVoltage(float voltage)
{
   m_voltage = voltage;
   setValid();
   return *this;
}

float BatteryStatus::getCurrent() const
{
   return m_current;
}

BatteryStatus& BatteryStatus::setCurrent(float current)
{
   m_current = current;
   setValid();
   return *this;
}

uint8_t BatteryStatus::getSecurityByte() const
{
   return m_security_byte;
}

uint8_t BatteryStatus::getMultiplexByte() const
{
   return m_multiplex_byte;
}

unsigned BatteryStatus::getUsableSOC() const
{
   return m_usable_soc;
}

void BatteryStatus::toStream(logging::ostream& os) const
{
   os << "BatteryStatus: ";
   if (valid())
   {
      os << "Current=" << getCurrent() << "A "
         << "Voltage=" << getVoltage() << "V "
         << "Usable SOC=" << getUsableSOC() << "% "
         << "SecurityByte=0x" << logging::HexByte(getSecurityByte())
         << " MultiplexByte=0x" << logging::HexByte(getMultiplexByte());
   }
   else
   {
      os << "invalid";
   }
}

}
}
}
