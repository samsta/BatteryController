/* SPDX-License-Identifier: GPL-3.0-or-later */
/* This is TSUN information and needs changing */
#ifndef _CAN_MESSAGES_TSUN_IDS_HPP
#define _CAN_MESSAGES_TSUN_IDS_HPP

namespace can {
namespace messages {
namespace TSUN {

enum Ids {
   ID_INVERTER_INFO_REQUEST = 0x4200,
   ID_INVERTER_SLEEP_AWAKE_COMMAND = 0x8200,
   ID_INVERTER_CHARGE_DISCHARGE_COMMAND = 0x8210,

   ID_BATTERY_INFO = 0x4211, // Voltage Current(I) Temperature SOC SOH
   ID_BATTERY_LIMITS = 0x4221,
   ID_BATTERY_CELL_VOLT_INFO = 0x4231,
   ID_BATTERY_CELL_TEMP_INFO = 0x4241,
   ID_BATTERY_STATUS = 0x4251,
   ID_BATTERY_MODULE_VOLT_INFO = 0x4261,
   ID_BATTERY_MODULE_TEMP_INFO = 0x4271,
   ID_BATTERY_FORBIDDEN = 0x4281
};

}
}
}

#endif
