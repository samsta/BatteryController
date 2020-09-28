/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_BATTERYMEASUREMENTS_HPP
#define _CAN_MESSAGES_SMA_BATTERYMEASUREMENTS_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class BatteryMeasurements: public StandardDataFrame
{
public:
   BatteryMeasurements();
   BatteryMeasurements(float voltage,
                       float current,
                       float temperature,
                       unsigned state,
                       unsigned inverter_control_flags);
   
   BatteryMeasurements& setVoltage(float voltage);
   BatteryMeasurements& setCurrent(float current);
   BatteryMeasurements& setTemperature(float temperature);
   BatteryMeasurements& setState(unsigned state);
   BatteryMeasurements& setInverterControlFlags(unsigned inverter_control_flags);
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYMEASUREMENTS_HPP
