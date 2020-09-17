/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_LARGEDATAFRAME_HPP_
#define _CAN_LARGEDATAFRAME_HPP_

#include "can/DataFrame.hpp"

namespace can {

class LargeDataFrame: public DataFrame
{
public:
   LargeDataFrame(uint32_t id, unsigned size);
   LargeDataFrame(uint32_t id, const uint8_t* data, unsigned size);
   virtual ~LargeDataFrame();

   void append(const uint8_t* data, unsigned size);
   virtual const uint8_t* data() const;
   virtual uint8_t* data();
   virtual bool valid() const;

private:
   uint8_t* m_data;
   unsigned m_cur_size;
};

}

#endif // _CAN_LARGEDATAFRAME_HPP_
