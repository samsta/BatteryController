/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SINEX_INVERTERHEARTBEAT_HPP
#define _CAN_MESSAGES_SINEX_INVERTERHEARTBEAT_HPP

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/SINEX/Ids.hpp"

namespace can {
namespace messages {
namespace SINEX {

class InverterHeartbeat: public Message
{
public:
   InverterHeartbeat(const DataFrame&);
   InverterHeartbeat();
   uint16_t getHeartbeatValue() const;
   virtual void toStream(logging::ostream&) const;

private:
   uint16_t m_heartbeat_value;
};

}
}
}

#endif // _CAN_MESSAGES_SINEX_INVERTERHEARTBEAT_HPP
