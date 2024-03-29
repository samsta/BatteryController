/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_INVERTERCHARGEDISCHARGECMD_HPP_
#define CAN_MESSAGES_TSUN_INVERTERCHARGEDISCHARGECMD_HPP_

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class InverterChargeDischargeCmd: public Message
{
public:
   enum Command {
      INVALID = 0,
      CHARGE = 1,
      DISCHARGE = 2
   };

   InverterChargeDischargeCmd(const DataFrame&);
   Command getCommand() const;
   virtual void toStream(logging::ostream&) const;

private:
   Command m_command;
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_INVERTERCHARGEDISCHARGECMD_HPP_ */
