/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_TESLA_TS_CUR_ENERGY_HPP_
#define CAN_MESSAGES_TESLA_TS_CUR_ENERGY_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/Tesla/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class TSCurrentEnergy: public Message
{
public:
   TSCurrentEnergy();
   TSCurrentEnergy(const DataFrame& f);

   float getChargeCurrentLimit() const { return m_charge_current_limit;};
   float getDischargeCurrentLimit() const { return m_discharge_current_limit;};
   float getStoredEnergy() const { return m_stored_energy;};
   float getBatteryCapacity() const { return m_battery_capacity;};
   virtual void toStream(logging::ostream&) const;

private:
   float m_charge_current_limit;
   float m_discharge_current_limit;
   float m_stored_energy;
   float m_battery_capacity;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_CUR_ENERGY_HPP_ */
