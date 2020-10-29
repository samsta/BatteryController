/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_PACKTEMPERATURES_HPP_
#define _CAN_MESSAGES_NISSAN_PACKTEMPERATURES_HPP_

#include "can/messages/Nissan/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

/*
 * Pack temperatures as described on https://www.mynissanleaf.com/viewtopic.php?f=44&t=11676
 *
 * We only use the 8 bit temperatures and ignore the 16bit raw A/D readings.
 *
 * Note that this is using aggregated frames as provided by the FrameAggregator.
 */
class PackTemperatures: public Message
{
public:
   PackTemperatures(const DataFrame& f);
   PackTemperatures();

   float getTemperature(unsigned pack_index) const;
   PackTemperatures& setTemperature(unsigned pack_index, float temperature);

   virtual void toStream(logging::ostream&) const;

   enum Constants {
      NUM_SENSORS = 4
   };

private:
   float m_temperatures[NUM_SENSORS];
};

}
}
}

#endif // _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_
