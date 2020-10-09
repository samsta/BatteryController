/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "InverterCommand.hpp"
#include "Ids.hpp"
#include <math.h>

namespace can {
namespace messages {
namespace SMA {

InverterCommand::InverterCommand(const DataFrame& frame):
      m_command(),
      m_voltage(),
      m_current(),
      m_temperature(),
      m_valid(false)
{
   if (frame.id() != ID_INVERTER_COMMAND) return;
   if (frame.size() != 8) return;

   m_command = frame.data()[6] == CONNECT ? CONNECT : DISCONNECT;
   m_voltage = frame.getUnsignedShort(0) * 0.1;
   m_current = frame.getSignedShort(2) * 0.1;
   m_temperature = frame.getSignedShort(4) * 0.1;
   
   m_valid = true;
}

InverterCommand::Command InverterCommand::getCommand() const
{
   return m_command;
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

bool InverterCommand::valid() const
{
   return m_valid;
}

logging::ostream& operator<<(logging::ostream& os, const InverterCommand& command)
{
   os << "InverterCommand: ";

   if (not command.valid()) return os << "invalid";

   os << "Command=" << command.getCommand()
      << " MeasuredBatteryVoltage=" << command.getMeasuredBatteryVoltage()
      << "V MeasuredBatteryCurrent=" << command.getMeasuredBatteryCurrent()
      << "A MeasuredBatteryTemperature=" << command.getMeasuredBatteryTemperature()
      << "degC";
   return os;
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

