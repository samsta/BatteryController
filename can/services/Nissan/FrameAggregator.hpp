/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_SERVICES_NISSAN_FRAMEAGGREGATOR_HPP
#define _CAN_SERVICES_NISSAN_FRAMEAGGREGATOR_HPP

#include <stdint.h>
#include "can/FrameSink.hpp"

namespace can {

class LargeDataFrame;

namespace services {
namespace Nissan {

/*
 * This class aggregates Nissan's multi-frame messages into
 * a single large frame. The only message ID with this
 * requirement at this stage is 0x7bb, the LBC Data Reply.
 *
 * https://www.mynissanleaf.com/viewtopic.php?f=44&t=11676
 * describes how the various groups consist of several lines
 * in individual frames which is what our aggregation method
 * is based on.
 *
 * All other messages are passed to the output as is.
 */
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
