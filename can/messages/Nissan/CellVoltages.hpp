/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_
#define _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_

#include "can/messages/Nissan/Message.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

/*
 * Cell voltages as described on https://www.mynissanleaf.com/viewtopic.php?f=44&t=11676
 *
 * Note that this is using aggregated frames as provided by the FrameAggregator.
 */
class CellVoltages: public Message
{
public:
   CellVoltages(const DataFrame& f);
   CellVoltages();

   float getVoltage(unsigned cell_index) const;
   CellVoltages& setVoltage(unsigned cell_index, float voltage);

   virtual void toStream(logging::ostream&) const;
   
   enum Constants {
      NUM_CELLS = 96
   };

private:
   float m_voltages[NUM_CELLS];
};

}
}
}

#endif // _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_
