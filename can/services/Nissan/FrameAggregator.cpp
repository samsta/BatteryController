/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "FrameAggregator.hpp"
#include "can/messages/Nissan/Ids.hpp"
#include "can/LargeDataFrame.hpp"

namespace can {
namespace services {
namespace Nissan {

namespace {

const uint8_t INITIAL_SEQ_NUM = 0x10;
const uint8_t LOWEST_CONTINUATION_SEQ_NUM = 0x20;

void incrementSequenceNumber(uint8_t& n)
{
   n = LOWEST_CONTINUATION_SEQ_NUM + ((n+1) & 0xf);
}

}

FrameAggregator::FrameAggregator(FrameSink& output):
   m_output(output),
   m_aggregated_frame(nullptr),
   m_expected_sequence_number(INITIAL_SEQ_NUM)
{
}

FrameAggregator::~FrameAggregator()
{
   delete m_aggregated_frame;
}

void FrameAggregator::restartAggregation()
{
   delete m_aggregated_frame;
   m_aggregated_frame = nullptr;
   m_expected_sequence_number = INITIAL_SEQ_NUM;
}

void FrameAggregator::sink(const DataFrame& input)
{
   if (input.id() != messages::Nissan::ID_LBC_DATA_REPLY)
   {
      m_output.sink(input);
      return;
   }

   if (input.data()[0] != m_expected_sequence_number)
   {
      restartAggregation();
      return;
   }

   if (not m_aggregated_frame)
   {
      m_aggregated_frame = new LargeDataFrame(input.id(), input.data()[1]);
      m_aggregated_frame->append(&input.data()[2], input.size() - 2);
   }
   else
   {
      m_aggregated_frame->append(&input.data()[1], input.size() - 1);
   }

   incrementSequenceNumber(m_expected_sequence_number);

   if (m_aggregated_frame->valid())
   {
      m_output.sink(*m_aggregated_frame);
      restartAggregation();
   }
}



}
}
}
