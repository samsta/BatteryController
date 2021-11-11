/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_IDS_HPP
#define _CAN_MESSAGES_NISSAN_IDS_HPP

namespace can {
namespace messages {
namespace Nissan {

enum Ids {
   ID_BATTERY_STATUS   = 0x1db,
   ID_LBC_DATA_REQUEST = 0x79b,
   ID_LBC_DATA_REPLY   = 0x7bb,
   ID_LBC_POWER_LIMITS = 0x1dc,
   ID_LBC_VOLTS_CURRENT = 0x1db,
   ID_LBC_VCM_DIAG_50B  = 0x50b,
   ID_LBC_HCM_CLOCK_50C = 0x50c,
   ID_LBC_STATUS_55B    = 0x55b,
   ID_LBC_HEARTBEAT_11A = 0x11A

};

enum LbcDataGroups {
   GROUP_NONE = 0,
   GROUP_BATTERY_STATE = 1,
   GROUP_CELL_VOLTAGES = 2,
   GROUP_CELL_VOLTAGE_RANGE = 3,
   GROUP_PACK_TEMPERATURES = 4
};

}
}
}

#endif
