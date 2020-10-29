/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_MESSAGE_HPP
#define _CAN_MESSAGES_NISSAN_MESSAGE_HPP

#include "Ids.hpp"

namespace can {
namespace messages {
namespace Nissan {

class Message {
public:
   Message(Ids id, LbcDataGroups data_group);
   virtual ~Message(){}
   
   bool valid() const;
   LbcDataGroups dataGroup() const;
   Ids id() const;
   
protected:
   void setValid();
   
private:
   bool m_valid;
   LbcDataGroups m_data_group;
   Ids m_id;
};

}
}
}

#endif // _CAN_MESSAGES_NISSAN_MESSAGE_HPP
