/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_TSUN_ENSEMBLEINFORMATION_HPP
#define _CAN_MESSAGES_TSUN_ENSEMBLEINFORMATION_HPP

#include "can/DataFrame.hpp"
#include "can/messages/Message.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class EnsembleInformation: public Message
{
public:

   EnsembleInformation(const DataFrame&);
   virtual void toStream(logging::ostream&) const {};
   virtual bool valid() const;
};

}
}
}

#endif // _CAN_MESSAGES_TSUN_ENSEMBLEINFORMATION_HPP
