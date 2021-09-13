/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYCELLTEMPINFO_HPP_
#define CAN_MESSAGES_TSUN_BATTERYCELLTEMPINFO_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryCellTempInfo: public StandardDataFrame
{
public:
   BatteryCellTempInfo();
   BatteryCellTempInfo(float max_single_cell_temperature,
                       float min_single_cell_temperature,
                       float max_cell_temperature_number,
                       float min_cell_temperature_number);
   BatteryCellTempInfo& setMaxSingleCellTemp(float max_single_cell_temperature);
   BatteryCellTempInfo& setMinSingleCellTemp(float min_single_cell_temperature);
   BatteryCellTempInfo& setMaxCellTempNumber(float max_cell_temperature_number);
   BatteryCellTempInfo& setMinCellTempNumber(float min_cell_temperature_number);
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYCELLTEMPINFO_HPP_ */
