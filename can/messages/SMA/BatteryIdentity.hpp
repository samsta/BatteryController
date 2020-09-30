/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYIDENTITY_HPP
#define _CAN_MESSAGES_SMA_BATTERYIDENTITY_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatteryIdentity: public StandardDataFrame
{
public:
   BatteryIdentity();
   BatteryIdentity(uint32_t serial_number,
                   uint32_t manufacturing_date_unix_time);
   
   BatteryIdentity& setSerialNumber(uint32_t serial_number);
   BatteryIdentity& setManufacturingDateUnixTime(uint32_t unix_time);
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYIDENTITY_HPP
