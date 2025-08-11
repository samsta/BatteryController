/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYMODULECAPACITY_HPP_
#define CAN_MESSAGES_TSUN_BATTERYMODULECAPACITY_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryModCapacity: public StandardDataFrame
{
public:
   BatteryModCapacity();
   BatteryModCapacity(float total_cells,
                     float mods_in_series,
                     float cells_per_mod,
                     float voltage_level,
                     float ah_capacity);

   BatteryModCapacity& setTotalCellAmount(float total_cells);
   BatteryModCapacity& setModulesInSeries(float mods_in_series);
   BatteryModCapacity& setCellsPerModule(float cells_per_mod);
   BatteryModCapacity& setVoltageLevel(float voltage_level);
   BatteryModCapacity& setAhCapacity(float ah_capacity);
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYMODULECAPACITY_HPP_ */
