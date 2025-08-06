/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYHWSWVERSION_HPP_
#define CAN_MESSAGES_TSUN_BATTERYHWSWVERSION_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryHWSWVersion: public StandardDataFrame
{
public:
   BatteryHWSWVersion();
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYHWSWVERSION_HPP_ */
