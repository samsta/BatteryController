/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryInfo::BatteryInfo():
   StandardDataFrame(ID_BATTERY_INFO, "0000000000000000")
{
}

BatteryInfo::BatteryInfo(float pile_voltage,
                         //float pile_current,
                         //float bms_2nd_temp,
                         unsigned soc,
                         unsigned soh):
   StandardDataFrame(ID_BATTERY_INFO, "0000000000000000")
{
   setPileVoltage(pile_voltage);
   //setPileCurrent(pile_current);
   //setBMS2ndTemp(bms_2nd_temp);
   setSOC(soc);
   setSOH(soh);
}

BatteryInfo& BatteryInfo::setPileVoltage(float pile_voltage)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(pile_voltage, 10), LSB_FIRST);
   // currently default pile_current to 0
   setUnsignedShort(2, 0, LSB_FIRST);
   // we will currently default bms_2nd_temp to 0
   setUnsignedShort(4, 0, LSB_FIRST);
   return *this;
}

//BatteryInfo& BatteryInfo::setPileCurrent(float pile_current)
//{
//   setUnsignedShort(2, limitScaledToUnsignedShort(pile_current + 3000, 10), LSB_FIRST);
//   return *this;
//}
//
//BatteryInfo& BatteryInfo::setBMS2ndTemp(float bms_2nd_temp)
//{
//   setUnsignedShort(4, limitScaledToUnsignedShort(bms_2nd_temp + 100, 10), LSB_FIRST);
//   return *this;
//}

BatteryInfo& BatteryInfo::setSOC(unsigned soc)
{
   setByte(6, limitValueToByte(soc));
   return *this;
}

BatteryInfo& BatteryInfo::setSOH(unsigned soh)
{
   setByte(7, limitValueToByte(soh));
   return *this;
}

}
}
}
