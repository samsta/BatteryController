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
   enum State {
      DISCONNECTED = 2,
      CONNECTED    = 3,
      FAULT        = 5
   };

   BatteryMeasurements();
   BatteryMeasurements(float voltage,
                       float current,
                       float temperature,
                       State state,
                       unsigned inverter_control_flags);
   
   BatteryMeasurements& setVoltage(float voltage);
   BatteryMeasurements& setCurrent(float current);
   BatteryMeasurements& setTemperature(float temperature);
   BatteryMeasurements& setState(State state);
   BatteryMeasurements& setInverterControlFlags(unsigned inverter_control_flags);
};

}
}
}

#endif // _CAN_MESSAGES_SMA_BATTERYMEASUREMENTS_HPP
