/* SPDX-License-Identifier: GPL-3.0-or-later */
/* This is TSUN information and needs changing */
#ifndef _CAN_MESSAGES_TSUN_IDS_HPP
#define _CAN_MESSAGES_TSUN_IDS_HPP

namespace can {
namespace messages {
namespace TSUN {

enum Ids {
   ID_INVERTER_REQUEST_INFORMATION = 0x4200,
   ID_INVERTER_SLEEP_AWAKE_COMMAND = 0x8200,
   ID_INVERTER_CHARGE_DISCHARGE_COMMAND = 0x8210,

   ID_BATTERY_INFO = 0x4210, // Voltage Current(I) Temperature SOC SOH
   ID_BATTERY_LIMITS = 0x4220,
   ID_BATTERY_CELL_VOLT_INFO = 0x4230,
   ID_BATTERY_CELL_TEMP_INFO = 0x4240,
   ID_BATTERY_STATUS_ERR_ALM = 0x4250,
   ID_BATTERY_CELL_V_LIMITS = 0x4260,
   ID_BATTERY_CELL_T_LIMITS = 0x4270
};

}
}
}

#endif
