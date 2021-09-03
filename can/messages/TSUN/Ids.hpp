/* SPDX-License-Identifier: GPL-3.0-or-later */
/* This is TSUN information and needs changing */
#ifndef _CAN_MESSAGES_TSUN_IDS_HPP
#define _CAN_MESSAGES_TSUN_IDS_HPP

namespace can {
namespace messages {
namespace TSUN {

enum Ids {
   ID_INVERTER_REQUEST_INFORMATION = 0x4200,
   ID_INVERTER_REQUEST_ENSEMBLE_INFO = 42000,
   ID_INVERTER_REQUEST_SYS_EQUIP_INFO = 42002
};

}
}
}

#endif
