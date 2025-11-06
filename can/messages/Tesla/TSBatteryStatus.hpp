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
   enum Battery_Status {
      OK,
      ZERO_MODULES,
      EXECESS_READ_ERROR_RATE,
      EXCESSS_CONSECUTIVE_READ_ERRORS,
      STARTUP,
      UNRECOGNIZED
   };

   TSBatteryStatus();
   TSBatteryStatus(const DataFrame& f);

   Battery_Status getBatteryStatus() const;
   virtual void toStream(logging::ostream&) const;



private:
   Battery_Status m_battery_status;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_BAT_STATUS_HPP_ */
