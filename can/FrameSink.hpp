/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_FRAMESINK_HPP
#define _CAN_FRAMESINK_HPP

namespace can {

class DataFrame;

class FrameSink
{
public:
   virtual ~FrameSink(){}
   virtual void sink(const DataFrame& ) = 0;
};

}

#endif // _CAN_FRAMESINK_HPP
