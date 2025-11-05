/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_TESLA_TS_VOLTS_HPP_
#define CAN_MESSAGES_TESLA_TS_VOLTS_HPP_

//#include "can/messages/Message.hpp"
#include "can/StandardDataFrame.hpp"
#include "can/messages/Tesla/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class TSCellVoltages: public Message
{
public:
   TSCellVoltages();
   TSCellVoltages(const DataFrame& f);

   float getMaxCellVoltage() const;
   float getMinCellVoltage() const;
   virtual void toStream(logging::ostream&) const;

private:
   float m_max_cell_voltage;
   float m_min_cell_voltage;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_VOLTS_HPP_ */
