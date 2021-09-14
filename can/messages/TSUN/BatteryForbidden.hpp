/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYFORBIDDEN_HPP_
#define CAN_MESSAGES_TSUN_BATTERYFORBIDDEN_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryForbidden: public StandardDataFrame
{
public:
   BatteryForbidden();
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYFORBIDDEN_HPP_ */
