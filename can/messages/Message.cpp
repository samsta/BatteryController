/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Message.hpp"

namespace can {
namespace messages {

Message::Message(unsigned id):
      m_valid(false),
      m_id(id)
{
}
  
bool Message::valid() const
{
   return m_valid;
}

unsigned Message::id() const
{
   return m_id;
}
   
void Message::setValid()
{
   m_valid = true;
}


logging::ostream& operator<<(logging::ostream& os, const Message& msg)
{
   msg.toStream(os);
   return os;
}

}
}

