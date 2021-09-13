/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYMODVOLTINFO_HPP_
#define CAN_MESSAGES_TSUN_BATTERYMODVOLTINFO_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryModVoltInfo: public StandardDataFrame
{
public:
   BatteryModVoltInfo();
   BatteryModVoltInfo(float max_single_module_voltage,
                       float min_single_module_voltage,
                       float max_module_voltage_number,
                       float min_module_voltage_number);
   BatteryModVoltInfo& setMaxSingleModuleVoltage(float max_single_module_voltage);
   BatteryModVoltInfo& setMinSingleModuleVoltage(float min_single_module_voltage);
   BatteryModVoltInfo& setMaxModuleVoltageNumber(float max_module_voltage_number);
   BatteryModVoltInfo& setMinModuleVoltageNumber(float min_module_voltage_number);
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYMODVOLTINFO_HPP_ */
