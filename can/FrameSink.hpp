#ifndef _CAN_FRAMESINK_HPP_
#define _CAN_FRAMESINK_HPP_

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
