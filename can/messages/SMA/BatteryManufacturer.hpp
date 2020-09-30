/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYMANUFACTURER_HPP
#define _CAN_MESSAGES_SMA_BATTERYMANUFACTURER_HPP

#include "can/messages/SMA/Ids.hpp"
#include "can/messages/SMA/String.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatteryManufacturer: public String
{
public:
   BatteryManufacturer(): String(ID_BATTERY_MANUFACTURER){}
   BatteryManufacturer(uint8_t index, const char* string): String(ID_BATTERY_MANUFACTURER, index, string){}
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYMANUFACTURER_HPP
