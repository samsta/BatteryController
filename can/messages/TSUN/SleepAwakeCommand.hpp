/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_SLEEPAWAKECOMMAND_HPP_
#define CAN_MESSAGES_TSUN_SLEEPAWAKECOMMAND_HPP_

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class SleepAwakeCommand: public Message
{
public:
   enum Command {
      ENTER_SLEEP    = 0x55,
      QUIT_SLEEP = 0xAA
   };


   SleepAwakeCommand(const DataFrame&);
   Command getCommand() const;
   virtual void toStream(logging::ostream&) const {};

private:
   Command m_command;
};

}
}
}


#endif /* CAN_MESSAGES_TSUN_SLEEPAWAKECOMMAND_HPP_ */
