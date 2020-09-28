/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYSYSTEMINFO_HPP
#define _CAN_MESSAGES_SMA_BATTERYSYSTEMINFO_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatterySystemInfo: public StandardDataFrame
{
public:
   BatterySystemInfo();
   BatterySystemInfo(uint32_t version,
                     float capacity_kwh,
                     uint8_t number_of_modules,
                     uint8_t manufacturer_id);
   
   BatterySystemInfo& setVersion(uint32_t version);
   BatterySystemInfo& setCapacityKwh(float capacity_kwh);
   BatterySystemInfo& setNumberOfModules(uint8_t number_of_modules);
   BatterySystemInfo& setManufacturerId(uint8_t manufacturer_id);
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYSYSTEMINFO_HPP
