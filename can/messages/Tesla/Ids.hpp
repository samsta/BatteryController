/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_TESLA_IDS_HPP
#define _CAN_MESSAGES_TESLA_IDS_HPP

namespace can {
namespace messages {
namespace Tesla {

enum Ids {
   // BC sends...

   // LBC sends...
   ID_TS_BATTERY_STATUS = 0x100,
   ID_TS_TEMPS          = 0x101,
   ID_TS_CELL_VOLT      = 0x102
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
