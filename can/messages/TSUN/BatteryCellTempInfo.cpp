/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryCellTempInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryCellTempInfo::BatteryCellTempInfo():
   StandardDataFrame(ID_BATTERY_CELL_TEMP_INFO, "0000000000000000")
{
}

BatteryCellTempInfo::BatteryCellTempInfo(float max_single_cell_temperature,
                       float min_single_cell_temperature,
                       float max_cell_temperature_number,
                       float min_cell_temperature_number):
   StandardDataFrame(ID_BATTERY_CELL_TEMP_INFO, "0000000000000000")
{
   setMaxSingleCellTemp(max_single_cell_temperature);
   setMinSingleCellTemp(min_single_cell_temperature);
   setMaxCellTempNumber(max_cell_temperature_number);
   setMinCellTempNumber(min_cell_temperature_number);
}

BatteryCellTempInfo& BatteryCellTempInfo::setMaxSingleCellTemp(float max_single_cell_temperature)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(max_single_cell_temperature + 100, 10), LSB_FIRST);
   return *this;
}

BatteryCellTempInfo& BatteryCellTempInfo::setMinSingleCellTemp(float min_single_cell_temperature)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(min_single_cell_temperature + 100, 10), LSB_FIRST);
   return *this;
}

BatteryCellTempInfo& BatteryCellTempInfo::setMaxCellTempNumber(float max_cell_temperature_number)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(max_cell_temperature_number, 1), LSB_FIRST);
   return *this;
}

BatteryCellTempInfo& BatteryCellTempInfo::setMinCellTempNumber(float min_cell_temperature_number)
{
   setUnsignedShort(6, limitScaledToUnsignedShort(min_cell_temperature_number, 1), LSB_FIRST);
   return *this;
}

}
}
}
