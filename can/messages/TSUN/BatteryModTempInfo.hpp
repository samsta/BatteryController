/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_MESSAGES_TSUN_BATTERYMODTEMPINFO_HPP_
#define CAN_MESSAGES_TSUN_BATTERYMODTEMPINFO_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

class BatteryModTempInfo: public StandardDataFrame
{
public:
   BatteryModTempInfo();
   BatteryModTempInfo(float max_single_module_tempage,
                       float min_single_module_tempage,
                       float max_module_tempage_number,
                       float min_module_tempage_number);
   BatteryModTempInfo& setMaxSingleModuleTemp(float max_single_module_tempage);
   BatteryModTempInfo& setMinSingleModuleTemp(float min_single_module_tempage);
   BatteryModTempInfo& setMaxModuleTempNumber(float max_module_tempage_number);
   BatteryModTempInfo& setMinModuleTempNumber(float min_module_tempage_number);
};

}
}
}

#endif /* CAN_MESSAGES_TSUN_BATTERYMODTEMPINFO_HPP_ */
