/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "BatteryCellVoltInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"
#include "util.hpp"

using namespace util;

namespace can {
namespace messages {
namespace TSUN {

BatteryCellVoltInfo::BatteryCellVoltInfo():
   StandardDataFrame(ID_BATTERY_CELL_VOLT_INFO, "0000000000000000")
{
}

BatteryCellVoltInfo::BatteryCellVoltInfo(float max_single_cell_voltage,
                       float min_single_cell_voltage,
                       float max_cell_voltage_number,
                       float min_cell_voltage_number):
   StandardDataFrame(ID_BATTERY_CELL_VOLT_INFO, "0000000000000000")
{
   setMaxSingleCellVoltage(max_single_cell_voltage);
   setMinSingleCellVoltage(min_single_cell_voltage);
   setMaxCellVoltageNumber(max_cell_voltage_number);
   setMinCellVoltageNumber(min_cell_voltage_number);
}

BatteryCellVoltInfo& BatteryCellVoltInfo::setMaxSingleCellVoltage(float max_single_cell_voltage)
{
   setUnsignedShort(0, limitScaledToUnsignedShort(max_single_cell_voltage, 1000), LSB_FIRST);
   return *this;
}

BatteryCellVoltInfo& BatteryCellVoltInfo::setMinSingleCellVoltage(float min_single_cell_voltage)
{
   setUnsignedShort(2, limitScaledToUnsignedShort(min_single_cell_voltage, 1000), LSB_FIRST);
   return *this;
}

BatteryCellVoltInfo& BatteryCellVoltInfo::setMaxCellVoltageNumber(float max_cell_voltage_number)
{
   setUnsignedShort(4, limitScaledToUnsignedShort(max_cell_voltage_number, 1), LSB_FIRST);
   return *this;
}

BatteryCellVoltInfo& BatteryCellVoltInfo::setMinCellVoltageNumber(float min_cell_voltage_number)
{
   setUnsignedShort(6, limitScaledToUnsignedShort(min_cell_voltage_number, 1), LSB_FIRST);
   return *this;
}

}
}
}
