/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_BATTERYSTATUS_HPP
#define _CAN_MESSAGES_NISSAN_BATTERYSTATYS_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/Nissan/Ids.hpp"
#include "logging/stream.hpp"

namespace can {
namespace messages {
namespace Nissan {

class BatteryStatus
{
public:
   BatteryStatus();
   BatteryStatus(const DataFrame&);
   
   float getVoltage() const;
   float getCurrent() const;
   uint8_t getSecurityByte() const;
   uint8_t getMultiplexByte() const;

   bool valid() const;
private:
   float m_voltage;
   float m_current;
   uint8_t m_security_byte;
   uint8_t m_multiplex_byte;
   bool m_valid;
};

logging::ostream& operator<<(logging::ostream&, const BatteryStatus&);

}
}
}

#endif // _CAN_MESSAGES_NISSAN_BATTERYSTATYS_HPP
