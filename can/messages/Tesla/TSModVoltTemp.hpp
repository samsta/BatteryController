/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_TESLA_TS_MOD_VOLT_TEMP_HPP_
#define CAN_MESSAGES_TESLA_TS_MOD_VOLT_TEMP_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/Tesla/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class TSModVoltTemp: public Message
{
public:
   struct VoltTempData {
      uint16_t module_num;
      float voltage;
      float temperatures[2];
   };

   TSModVoltTemp();
   TSModVoltTemp(const DataFrame& f);

   const VoltTempData& getVoltTempData() const { return m_volt_temp_data; }
   virtual void toStream(logging::ostream&) const;

private:
   VoltTempData m_volt_temp_data;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_MOD_VOLT_TEMP_HPP_ */
