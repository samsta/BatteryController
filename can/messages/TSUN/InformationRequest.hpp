/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_TSUN_INFORMATIONREQUEST_HPP
#define _CAN_MESSAGES_TSUN_INFORMATIONREQUEST_HPP

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class InformationRequest: public Message
{
public:
	enum InfoType {
	      ENSEMBLE    = 0,
	      SYSTEMEQUIPMENT = 2
	   };


   InformationRequest(const DataFrame&);
   InfoType getInfoType() const;
   virtual void toStream(logging::ostream&) const {};

private:
   InfoType m_infotype;
};

}
}
}

#endif // _CAN_MESSAGES_TSUN_INFORMATIONREQUEST_HPP
