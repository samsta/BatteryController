/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "InverterCommand.hpp"
#include "Ids.hpp"
#include <math.h>

namespace can {
namespace messages {
namespace SMA {

InverterCommand::InverterCommand(const DataFrame& frame):
      Message(ID_INVERTER_COMMAND),
      m_command(),
      m_voltage(),
      m_current(),
      m_temperature()
{
   if (frame.id() != ID_INVERTER_COMMAND) return;
   if (frame.size() != 8) return;

   m_command = frame.data()[6] == CONNECT ? CONNECT : DISCONNECT;
   m_voltage = frame.getUnsignedShort(0) * 0.1;
   m_current = frame.getSignedShort(2) * 0.1;
   m_temperature = frame.getSignedShort(4) * 0.1;
   
   setValid();
}

InverterCommand::InverterCommand():
      Message(ID_INVERTER_COMMAND),
      m_command(),
      m_voltage(),
      m_current(),
      m_temperature()
{
}

InverterCommand::Command InverterCommand::getCommand() const
{
   return m_command;
}

InverterCommand& InverterCommand::setCommand(Command command)
{
   m_command = command;
   setValid();
   return *this;
}
      

float InverterCommand::getMeasuredBatteryVoltage() const
{
   return m_voltage;
}

float InverterCommand::getMeasuredBatteryCurrent() const
{
   return m_current;
}

float InverterCommand::getMeasuredBatteryTemperature() const
{
   return m_temperature;
}

void InverterCommand::toStream(logging::ostream& os) const
{
   os << "InverterCommand: ";

   if (not valid())
   {
      os << "invalid";
   }
   else
   {
      os << "Command=" << getCommand()
         << " MeasuredBatteryVoltage=" << getMeasuredBatteryVoltage()
         << "V MeasuredBatteryCurrent=" << getMeasuredBatteryCurrent()
         << "A MeasuredBatteryTemperature=" << getMeasuredBatteryTemperature()
         << "degC";
   }
}

logging::ostream& operator<<(logging::ostream& os, const InverterCommand::Command& c)
{
   switch(c)
   {
   case InverterCommand::CONNECT:    return os << "CONNECT";
   case InverterCommand::DISCONNECT: return os << "DISCONNECT";
   default:                          return os << "INVALID(" << unsigned(c) << ")";
   }
}

}
}
}

