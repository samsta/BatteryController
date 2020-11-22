/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_INVERTERMANUFACTURER_HPP
#define _CAN_MESSAGES_SMA_INVERTERMANUFACTURER_HPP

#include "can/messages/SMA/Ids.hpp"
#include "can/messages/SMA/String.hpp"

namespace can {
namespace messages {
namespace SMA {

class InverterManufacturer: public String
{
public:
   InverterManufacturer(const DataFrame&f): String(ID_INVERTER_MANUFACTURER, f){} 
   InverterManufacturer(): String(ID_INVERTER_MANUFACTURER){}
   InverterManufacturer(uint8_t index, const char* string): String(ID_INVERTER_MANUFACTURER, index, string){}
};

}
}
}

#endif // _CAN_MESSAGES_SMA_INVERTERMANUFACTURER_HPP
