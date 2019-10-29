/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_STANDARDDATAFRAME_HPP
#define _CAN_STANDARDDATAFRAME_HPP

#include <stdint.h>
#include "can/DataFrame.hpp"
#include "logging/stream.hpp"

namespace can {

class StandardDataFrame: public DataFrame
{
public:
   StandardDataFrame();
   StandardDataFrame(const char*);
   StandardDataFrame(uint32_t id, const uint8_t data[], unsigned size = 8);

   virtual const uint8_t* data() const;
private:
   uint8_t m_data[8];
};

}

#endif // _CAN_DATAFRAME_HPP
