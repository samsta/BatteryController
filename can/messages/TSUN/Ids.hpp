/* SPDX-License-Identifier: GPL-3.0-or-later */
/* This is TSUN information and needs changing */
#ifndef _CAN_MESSAGES_TSUN_IDS_HPP
#define _CAN_MESSAGES_TSUN_IDS_HPP

namespace can {
namespace messages {
namespace TSUN {

// these are extended CAN Ids, they must have the highest bit set to indicate such (0x80000000)
enum Ids {
   ID_INVERTER_INFO_REQUEST = 0x00004200,
   ID_INVERTER_SLEEP_AWAKE_COMMAND = 0x00008200,
   ID_INVERTER_CHARGE_DISCHARGE_COMMAND = 0x00008210,

   ID_BATTERY_INFO = 0x00004211, // Voltage Current(I) Temperature SOC SOH
   ID_BATTERY_LIMITS = 0x00004221,
   ID_BATTERY_CELL_VOLT_INFO = 0x00004231,
   ID_BATTERY_CELL_TEMP_INFO = 0x00004241,
   ID_BATTERY_STATUS = 0x00004251,
   ID_BATTERY_MODULE_VOLT_INFO = 0x00004261,
   ID_BATTERY_MODULE_TEMP_INFO = 0x00004271,
   ID_BATTERY_FORBIDDEN = 0x00004281
};

}
}
}

#endif
