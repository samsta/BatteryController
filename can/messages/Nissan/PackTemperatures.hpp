/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_PACKTEMPERATURES_HPP_
#define _CAN_MESSAGES_NISSAN_PACKTEMPERATURES_HPP_

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
class PackTemperatures
{
public:
   PackTemperatures(const DataFrame& f);
   PackTemperatures();

   bool valid() const;

   float getTemperature(unsigned pack_index) const;
   PackTemperatures& setTemperature(unsigned pack_index, float temperature);

   enum Constants {
      NUM_PACKS = 4
   };

private:
   bool m_valid;
   float m_temperatures[NUM_PACKS];
};

logging::ostream& operator<<(logging::ostream&, const PackTemperatures&);

}
}
}

#endif // _CAN_MESSAGES_NISSAN_CELLVOLTAGES_HPP_
