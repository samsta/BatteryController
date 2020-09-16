/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_IDS_HPP
#define _CAN_MESSAGES_SMA_IDS_HPP

namespace can {
namespace messages {
namespace SMA {

enum Ids {
   ID_BATTERY_ALARMS       = 0x158,
   ID_BATTERY_ERRORS       = 0x198,
   ID_BATTERY_EVENTS       = 0x218,
   ID_BATTERY_LIMITS       = 0x358,
   ID_BATTERY_STATE        = 0x3d8,
   ID_BATTERY_MEASUREMENTS = 0x4d8,
   ID_BATTERY_SYSTEM_INFO  = 0x558,
   ID_BATTERY_IDENTIY      = 0x598,
   ID_BATTERY_MANUFACTURER = 0x5d8,
   ID_BATTERY_NAME         = 0x618
};

}
}
}

#endif
