/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_IDS_HPP
#define _CAN_MESSAGES_NISSAN_IDS_HPP

namespace can {
namespace messages {
namespace Nissan {

enum Ids {
   ID_LBC_DATA_REPLY = 0x7bb
};

enum LbcDataGroups {
   GROUP_BATTERY_STATE = 1,
   GROUP_CELL_VOLTAGES = 2,
   GROUP_PACK_TEMPERATURES = 4
};

}
}
}

#endif
