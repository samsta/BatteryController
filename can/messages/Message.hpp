/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_MESSAGE_HPP
#define _CAN_MESSAGES_MESSAGE_HPP

#include "logging/stream.hpp"

namespace can {
namespace messages {

class Message {
public:
   Message(unsigned id);
   virtual ~Message(){}
   
   bool valid() const;
   unsigned id() const;

   virtual void toStream(logging::ostream&) const = 0;
   
protected:
   void setValid();
   
private:
   bool m_valid;
   unsigned m_id;
};

logging::ostream& operator<<(logging::ostream&, const Message&);

}
}

#endif // _CAN_MESSAGES_SMA_MESSAGE_HPP
