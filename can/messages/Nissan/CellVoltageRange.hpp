/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_CELLVOLTAGERANGE_HPP_
#define _CAN_MESSAGES_NISSAN_CELLVOLTAGERANGE_HPP_

#include "can/messages/Nissan/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

/*
 * Cell voltage range as described on https://www.mynissanleaf.com/viewtopic.php?f=44&t=11676
 *
 * Note that this is using aggregated frames as provided by the FrameAggregator.
 */
class CellVoltageRange: public Message
{
public:
   CellVoltageRange(const DataFrame& f);
   CellVoltageRange();

   float getMin() const;
   CellVoltageRange& setMin(float voltage);

   float getMax() const;
   CellVoltageRange& setMax(float voltage);

private:
   float m_min_voltage;
   float m_max_voltage;
};

logging::ostream& operator<<(logging::ostream&, const CellVoltageRange&);

}
}
}

#endif // _CAN_MESSAGES_NISSAN_CELLVOLTAGERANGE_HPP_
