/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryModCapacity.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryModCapacity::BatteryModCapacity():
   StandardDataFrame(ID_BATTERY_MODULE_CAPACITY, "0000000000000000")
{
   // TODO look up what Dala (the Great) sends for this
}

}
}
}


