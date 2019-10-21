#ifndef _CAN_SERVICES_NISSAN_FRAMEAGGREGATOR_HPP
#define _CAN_SERVICES_NISSAN_FRAMEAGGREGATOR_HPP

#include <stdint.h>
#include "can/FrameSink.hpp"

namespace can {

class LargeDataFrame;

namespace services {
namespace Nissan {

class FrameAggregator: public FrameSink
{
public:
   FrameAggregator(FrameSink& output);
   ~FrameAggregator();

   virtual void sink(const DataFrame& input);

private:
   void restartAggregation();

   FrameSink& m_output;
   LargeDataFrame* m_aggregated_frame;
   uint8_t m_expected_sequence_number;
};

}
}
}

#endif // _CAN_SERVICES_NISSAN_FRAMEAGGREGATOR_HPP
