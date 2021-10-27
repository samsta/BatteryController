/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_NISSAN_BATTERYVOLTSCURRENT_HPP_
#define CAN_MESSAGES_NISSAN_BATTERYVOLTSCURRENT_HPP_

#include "can/messages/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

class BatteryVoltsCurrent: public Message
{
public:
   BatteryVoltsCurrent(const DataFrame& f);
   BatteryVoltsCurrent();

   float getTotalVoltage() const;
   float getTotalCurrent() const;
   virtual void toStream(logging::ostream&) const;

private:
   float m_total_voltage;
   float m_total_current;
};

}
}
}
#endif /* CAN_MESSAGES_NISSAN_BATTERYVOLTSCURRENT_HPP_ */
