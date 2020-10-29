/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_MESSAGE_HPP
#define _CAN_MESSAGES_NISSAN_MESSAGE_HPP

#include "Ids.hpp"
#include "logging/stream.hpp"

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

   virtual void toStream(logging::ostream&) const = 0;
   
protected:
   void setValid();
   
private:
   bool m_valid;
   LbcDataGroups m_data_group;
   Ids m_id;
};

logging::ostream& operator<<(logging::ostream&, const Message&);

}
}
}

#endif // _CAN_MESSAGES_NISSAN_MESSAGE_HPP
