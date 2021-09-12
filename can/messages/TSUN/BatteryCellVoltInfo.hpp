/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYCELLVOLTINFO_HPP_
#define CAN_MESSAGES_TSUN_BATTERYCELLVOLTINFO_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryCellVoltInfo: public StandardDataFrame
{
public:
   BatteryCellVoltInfo();
   BatteryCellVoltInfo(float max_single_cell_voltage,
                       float min_single_cell_voltage,
                       float max_cell_voltage_number,
                       float min_cell_voltage_number);
   BatteryCellVoltInfo& setMaxSingleCellVoltage(float max_single_cell_voltage);
   BatteryCellVoltInfo& setMinSingleCellVoltage(float min_single_cell_voltage);
   BatteryCellVoltInfo& setMaxCellVoltageNumber(float max_cell_voltage_number);
   BatteryCellVoltInfo& setMinCellVoltageNumber(float min_cell_voltage_number);
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYCELLVOLTINFO_HPP_ */
