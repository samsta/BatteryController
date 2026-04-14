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
   struct CellVoltageData {
      uint16_t module_num;
      float voltages[6];
   };

   TSModCellVoltages();
   TSModCellVoltages(const DataFrame& f);

   uint16_t getModuleNum() const;
   float getCellVoltage(uint cell) const;
   const CellVoltageData& getCellVoltages() const { return m_cell_data; }
   virtual void toStream(logging::ostream&) const;

private:
   CellVoltageData m_cell_data;
};

}
}
}
#endif /* CAN_MESSAGES_TESLA_TS_MOD_VOLTS_HPP_ */
