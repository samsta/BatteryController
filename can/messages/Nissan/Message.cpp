/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Message.hpp"

namespace can {
namespace messages {
namespace Nissan {

Message::Message(Ids id, LbcDataGroups data_group):
      m_valid(false),
      m_data_group(data_group),
      m_id(id)
{
}
  
bool Message::valid() const
{
   return m_valid;
}

LbcDataGroups Message::dataGroup() const
{
   return m_data_group;
}

Ids Message::id() const
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
}

