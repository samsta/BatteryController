/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_
#define _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_

#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

/*
 * Cell voltages as described on https://www.mynissanleaf.com/viewtopic.php?f=44&t=11676
 *
 * Note that this is using aggregated frames as provided by the FrameAggregator.
 */
class CellVoltages
{
public:
   CellVoltages(const DataFrame& f);
   CellVoltages();

   bool valid() const;

   float getVoltage(unsigned cell_index) const;
   CellVoltages& setVoltage(unsigned cell_index, float voltage);

   enum Constants {
      NUM_CELLS = 96
   };

private:
   bool m_valid;
   float m_voltages[NUM_CELLS];
};

logging::ostream& operator<<(logging::ostream&, const CellVoltages&);

}
}
}

#endif // _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_
