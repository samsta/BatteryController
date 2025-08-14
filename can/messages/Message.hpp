/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_MESSAGE_HPP
#define _CAN_MESSAGES_MESSAGE_HPP

#include "logging/stream.hpp"
#include <cstdint>

namespace can {
namespace messages {

class Message {
public:
   Message(uint32_t id);
   virtual ~Message(){}
   
   bool valid() const;
   uint32_t id() const;

   virtual void toStream(logging::ostream&) const = 0;
   
protected:
   void setValid();
   
private:
   bool m_valid;
   uint32_t m_id;
};

class MessageSink
{
public:
   virtual ~MessageSink(){}
   virtual void sink(const Message& msg) = 0;
};

logging::ostream& operator<<(logging::ostream&, const Message&);

}
}

#endif // CAN_MESSAGES_MESSAGE_HPP
