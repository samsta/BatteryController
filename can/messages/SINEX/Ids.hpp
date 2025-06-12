/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef _CAN_MESSAGES_SINEX_IDS_HPP
#define _CAN_MESSAGES_SINEX_IDS_HPP

namespace can {
namespace messages {
namespace SINEX {

// these are extended CAN Ids, they must have the highest bit set to indicate such (0x80000000)
enum Ids {
   ID_INVERTER_HEARTBEAT = 0x18160127,

   ID_BATTERY_LIMITS_ONE = 0X18102701,
   ID_BATTERY_LIMITS_TWO = 0x18112701,
   ID_BATTERY_STATUS =     0x18122201
};

}
}
}

#endif
