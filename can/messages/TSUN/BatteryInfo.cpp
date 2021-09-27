/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"
#include "math.h"


using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryInfo::BatteryInfo():
   StandardDataFrame(ID_BATTERY_INFO, "0000000000000000")
{
}

BatteryInfo::BatteryInfo(float pile_voltage,
                         float pile_current,
                         float bms_2nd_temp,
                         unsigned soc,
                         unsigned soh):
   StandardDataFrame(ID_BATTERY_INFO, "0000000000000000")
{
   setPileVoltage(pile_voltage);
   setPileCurrent(pile_current);
   setBMS2ndTemp(bms_2nd_temp);
   setSOC(soc);
   setSOH(soh);
}

BatteryInfo& BatteryInfo::setPileVoltage(float pile_voltage)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(pile_voltage, 10), LSB_FIRST);
   return *this;
}

BatteryInfo& BatteryInfo::setPileCurrent(float pile_current)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(pile_current + 3000, 10), LSB_FIRST);
   return *this;
}

BatteryInfo& BatteryInfo::setBMS2ndTemp(float bms_2nd_temp)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(bms_2nd_temp + 100, 10), LSB_FIRST);
   return *this;
}

BatteryInfo& BatteryInfo::setSOC(float soc)
{
   setByte(6, limitValueToByte(unsigned(round(soc))));
   return *this;
}

BatteryInfo& BatteryInfo::setSOH(float soh)
{
   setByte(7, limitValueToByte(unsigned(round(soh))));
   return *this;
}

}
}
}
