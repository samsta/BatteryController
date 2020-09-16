/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYERRORS_HPP
#define _CAN_MESSAGES_SMA_BATTERYERRORS_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace SMA {

class BatteryErrors: public StandardDataFrame
{
public:
   BatteryErrors(): StandardDataFrame(ID_BATTERY_ERRORS, "FFFFFFFFFFFFFFFF")
   {}
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYERRORS_HPP
