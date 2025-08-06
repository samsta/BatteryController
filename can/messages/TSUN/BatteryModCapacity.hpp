/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYMODULECAPACITY_HPP_
#define CAN_MESSAGES_TSUN_BATTERYMODULECAPACITY_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryModCapacity: public StandardDataFrame
{
public:
   BatteryModCapacity();
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYMODULECAPACITY_HPP_ */
