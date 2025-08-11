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
}

BatteryModCapacity::BatteryModCapacity(float total_cells,
                     float mods_in_series,
                     float cells_per_mod,
                     float voltage_level,
                     float ah_capacity):
   StandardDataFrame(ID_BATTERY_MODULE_CAPACITY, "0000000000000000")
{
   setTotalCellAmount(total_cells);
   setModulesInSeries(mods_in_series);
   setCellsPerModule(cells_per_mod);
   setVoltageLevel(voltage_level);
   setAhCapacity(ah_capacity);
}

BatteryModCapacity& BatteryModCapacity::setTotalCellAmount(float total_cells)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(total_cells, 1), LSB_FIRST);
   return *this;
}

BatteryModCapacity& BatteryModCapacity::setModulesInSeries(float mods_in_series)
{
   setByte(2, limitValueToByte( uint8_t(mods_in_series)));
   return *this;
}

BatteryModCapacity& BatteryModCapacity::setCellsPerModule(float cells_per_mod)
{
   setByte(3, limitValueToByte( uint8_t(cells_per_mod)));
   return *this;
}

BatteryModCapacity& BatteryModCapacity::setVoltageLevel(float voltage_level )
{
   setUnsignedShort(4, limitScaledToUnsignedShort(voltage_level, 1), LSB_FIRST);
   return *this;
}

BatteryModCapacity& BatteryModCapacity::setAhCapacity(float ah_capacity )
{
   setUnsignedShort(6, limitScaledToUnsignedShort(ah_capacity, 1), LSB_FIRST);
   return *this;
}

}
}
}


