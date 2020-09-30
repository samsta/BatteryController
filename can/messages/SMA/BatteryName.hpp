/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYNAME_HPP
#define _CAN_MESSAGES_SMA_BATTERYNAME_HPP

#include "can/messages/SMA/Ids.hpp"
#include "can/messages/SMA/String.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatteryName: public String
{
public:
   BatteryName(): String(ID_BATTERY_NAME){}
   BatteryName(uint8_t index, const char* string): String(ID_BATTERY_NAME, index, string){}
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYNAME_HPP
