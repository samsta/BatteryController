/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryHWSWVersion.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryHWSWVersion::BatteryHWSWVersion():
   StandardDataFrame(ID_BATTERY_HWSW_VERSION, "0010203015501600")
{
   // data populated as shown in the TSUN CAN Protocol Spec
   // TODO look up what Dala (the Great) sends for this
}

}
}
}


