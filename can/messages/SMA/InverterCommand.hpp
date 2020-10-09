/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_INVERTERCOMMAND_HPP
#define _CAN_MESSAGES_SMA_INVERTERCOMMAND_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class InverterCommand
{
public:
   enum Command {
      CONNECT    = 1,
      DISCONNECT = 0
   };

   InverterCommand(const DataFrame&);

   Command getCommand() const;
   float getMeasuredBatteryVoltage() const;
   float getMeasuredBatteryCurrent() const;
   float getMeasuredBatteryTemperature() const;

   bool valid() const;
private:
   Command m_command;
   float m_voltage;
   float m_current;
   float m_temperature;
   bool m_valid;
};

logging::ostream& operator<<(logging::ostream&, const InverterCommand&);
logging::ostream& operator<<(logging::ostream&, const InverterCommand::Command&);

}
}
}

#endif // _CAN_MESSAGES_SMA_INVERTERCOMMAND_HPP
