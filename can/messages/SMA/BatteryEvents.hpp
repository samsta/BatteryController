/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYEVENTS_HPP
#define _CAN_MESSAGES_SMA_BATTERYEVENTS_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace SMA {

class BatteryEvents: public StandardDataFrame
{
public:
   BatteryEvents(): StandardDataFrame(ID_BATTERY_EVENTS, "FFFFFFFFFFFFFFFF")
   {}
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYEVENTS_HPP
