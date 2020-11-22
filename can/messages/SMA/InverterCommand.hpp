/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_INVERTERCOMMAND_HPP
#define _CAN_MESSAGES_SMA_INVERTERCOMMAND_HPP

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class InverterCommand: public Message
{
public:
   enum Command {
      CONNECT    = 1,
      DISCONNECT = 0
   };

   InverterCommand(const DataFrame&);
   InverterCommand();

   Command getCommand() const;
   InverterCommand& setCommand(Command);
   
   float getMeasuredBatteryVoltage() const;
   float getMeasuredBatteryCurrent() const;
   float getMeasuredBatteryTemperature() const;

   virtual void toStream(logging::ostream&) const;
   
private:
   Command m_command;
   float m_voltage;
   float m_current;
   float m_temperature;
};

logging::ostream& operator<<(logging::ostream&, const InverterCommand::Command&);

}
}
}

#endif // _CAN_MESSAGES_SMA_INVERTERCOMMAND_HPP
