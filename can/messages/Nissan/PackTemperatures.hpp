/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_PACKTEMPERATURES_HPP_
#define _CAN_MESSAGES_NISSAN_PACKTEMPERATURES_HPP_

#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

class PackTemperatures
{
public:
   PackTemperatures(const DataFrame& f);

   bool valid() const;

   float getTemperature(unsigned pack_index) const;

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
