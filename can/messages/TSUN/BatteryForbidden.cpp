/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryForbidden.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryForbidden::BatteryForbidden():
   StandardDataFrame(ID_BATTERY_FORBIDDEN, "0000000000000000")
{
   // the inverter gave no response to this message in testing
   // so we will always send a blank message
}

}
}
}


