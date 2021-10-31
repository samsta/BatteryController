/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_NISSAN_BATTERYPOWERLIMITS_HPP_
#define CAN_MESSAGES_NISSAN_BATTERYPOWERLIMITS_HPP_

//#include "can/messages/Message.hpp"
#include "can/StandardDataFrame.hpp"
#include "can/messages/Nissan/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

class BatteryPowerLimits: public Message
{
public:
   BatteryPowerLimits();
   BatteryPowerLimits(const DataFrame& f);

   float getDischargePowerLimit_kW() const;
   BatteryPowerLimits& setDischargePowerLimit_kW(float power);
   float getChargePowerLimit_kW() const;
   BatteryPowerLimits& setChargePowerLimit_kW(float power);
   virtual void toStream(logging::ostream&) const;

private:
   float m_discharge_power_limit_kw;
   float m_charge_power_limit_kw;
};

}
}
}
#endif /* CAN_MESSAGES_NISSAN_BATTERYPOWERLIMITS_HPP_ */
