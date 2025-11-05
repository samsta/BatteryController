/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_TESLA_TS_BAT_STATUS_HPP_
#define CAN_MESSAGES_TESLA_TS_BAT_STATUS_HPP_

//#include "can/messages/Message.hpp"
#include "can/StandardDataFrame.hpp"
#include "can/messages/Tesla/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class TSBatteryStatus: public Message
{
public:
   TSBatteryStatus();
   TSBatteryStatus(const DataFrame& f);

   uint32_t getBatteryStatus() const;
   virtual void toStream(logging::ostream&) const;

private:
   uint32_t m_battery_status;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_BAT_STATUS_HPP_ */
