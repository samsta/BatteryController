/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_TSUN_SYSTEMEQUIPMENT_HPP
#define _CAN_MESSAGES_TSUN_SYSTEMEQUIPMENT_HPP

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class SystemEquipment: public Message
{
public:

   SystemEquipment(const DataFrame&);
   virtual void toStream(logging::ostream&) const {};
};

}
}
}

#endif // _CAN_MESSAGES_TSUN_SYSTEMEQUIPMENT_HPP
