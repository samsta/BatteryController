/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_NISSAN_BATTERYPOWERLIMITS_HPP_
#define CAN_MESSAGES_NISSAN_BATTERYPOWERLIMITS_HPP_

#include "can/messages/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

class BatteryPowerLimits: public Message
{
public:
   BatteryPowerLimits(const DataFrame& f);
   BatteryPowerLimits();

   float getDischargePowerLimit() const;
   float getChargePowerLimit() const;
   virtual void toStream(logging::ostream&) const;

private:
   float m_discharge_power_limit;
   float m_charge_power_limit;
};

}
}
}
#endif /* CAN_MESSAGES_NISSAN_BATTERYPOWERLIMITS_HPP_ */
