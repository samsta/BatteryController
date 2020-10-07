/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Message.hpp"

namespace can {
namespace messages {
namespace Nissan {

Message::Message(LbcDataGroups data_group):
      m_valid(false),
      m_data_group(data_group)
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
   
void Message::setValid()
{
   m_valid = true;
}

}
}
}

