/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryHWSWVersion.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryHWSWVersion::BatteryHWSWVersion():
   // value taken from Dala's Baterry Emulator PYTHON-CAN.cpp
   StandardDataFrame(ID_BATTERY_HWSW_VERSION, "0100020101020000")
{
}

}
}
}


