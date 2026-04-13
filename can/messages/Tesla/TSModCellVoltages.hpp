/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef CAN_MESSAGES_TESLA_TS_MOD_VOLTS_HPP_
#define CAN_MESSAGES_TESLA_TS_MOD_VOLTS_HPP_

#include "can/StandardDataFrame.hpp"
#include "can/messages/Tesla/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class TSModCellVoltages: public Message
{
public:
   TSModCellVoltages();
   TSModCellVoltages(const DataFrame& f);

   uint16_t getModuleNum() const;
   float getCellVoltage(uint cell) const;
   virtual void toStream(logging::ostream&) const;

private:
   float m_cell_voltage[6];
   uint16_t m_module_num;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_MOD_VOLTS_HPP_ */
