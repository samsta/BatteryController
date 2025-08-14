/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_TSUN_INVERTERINFOREQUEST_HPP
#define _CAN_MESSAGES_TSUN_INVERTERINFOREQUEST_HPP

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class InverterInfoRequest: public Message
{
public:
   enum InfoType {
      ENSEMBLE    = 0,
      SYSTEM_EQUIPMENT = 2
   };


   InverterInfoRequest(const DataFrame&);
   InverterInfoRequest();
   InfoType getInfoType() const;
   virtual void toStream(logging::ostream&) const;

private:
   InfoType m_info_type;
};

class InverterGeneral: public Message
{
public:
   InverterGeneral(const DataFrame&);
   // InverterGeneral();
   uint64_t getCANData() const;
   virtual void toStream(logging::ostream&) const;

private:
   uint64_t m_CAN_data;
};

}
}
}

#endif // _CAN_MESSAGES_TSUN_INVERTERINFOREQUEST_HPP
