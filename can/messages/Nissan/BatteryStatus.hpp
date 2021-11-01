/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_BATTERYSTATUS_HPP
#define _CAN_MESSAGES_NISSAN_BATTERYSTATUS_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/Nissan/Message.hpp"
#include "logging/stream.hpp"

namespace can {
namespace messages {
namespace Nissan {

class BatteryStatus: public Message
{
public:
   BatteryStatus();
   BatteryStatus(const DataFrame&);
   
   float getVoltage() const;
   BatteryStatus& setVoltage(float voltage);
   float getCurrent() const;
   BatteryStatus& setCurrent(float current);
   uint8_t getSecurityByte() const;
   uint8_t getMultiplexByte() const;
   float getUsableSOC() const;
   BatteryStatus& setUsableSOC(float current);

   virtual void toStream(logging::ostream&) const;

private:
   float m_voltage;
   float m_current;
   uint8_t m_security_byte;
   uint8_t m_multiplex_byte;
   float m_usable_soc;
};

}
}
}

#endif // _CAN_MESSAGES_NISSAN_BATTERYSTATUS_HPP
