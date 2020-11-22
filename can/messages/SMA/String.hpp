/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_STRING_HPP
#define _CAN_MESSAGES_SMA_STRING_HPP


#include "can/StandardDataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
class FrameSink;
namespace messages {
namespace SMA {

class String: public StandardDataFrame, public Message
{
public:
   String(Ids id, const DataFrame& f);
   String(Ids id);
   String(Ids id, uint8_t index, const char* string);

   using StandardDataFrame::id;
   
   virtual void toStream(logging::ostream&) const;
};

void sendString(FrameSink& sender, Ids id, const char* string);

}
}
}

#endif // _CAN_MESSAGES_SMA_STRING_HPP
