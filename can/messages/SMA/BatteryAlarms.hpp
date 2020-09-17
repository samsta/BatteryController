/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYALARMS_HPP
#define _CAN_MESSAGES_SMA_BATTERYALARMS_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatteryAlarms: public StandardDataFrame
{
public:
   BatteryAlarms(): StandardDataFrame(ID_BATTERY_ALARMS, "0000000000000000")
   {}
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYALARMS_HPP
