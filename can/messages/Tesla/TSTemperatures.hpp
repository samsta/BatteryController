/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_TESLA_TS_TEMPS_HPP_
#define CAN_MESSAGES_TESLA_TS_TEMPS_HPP_

//#include "can/messages/Message.hpp"
#include "can/StandardDataFrame.hpp"
#include "can/messages/Tesla/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class TSTemperatures: public Message
{
public:
   TSTemperatures();
   TSTemperatures(const DataFrame& f);

   float getMaxTemperature() const;
   float getMinTempeature() const;
   float getAvgTemperature() const;
   virtual void toStream(logging::ostream&) const;

private:
   float m_max_temperature;
   float m_min_temperature;
   float m_average_temperature;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_TEMPS_HPP_ */
