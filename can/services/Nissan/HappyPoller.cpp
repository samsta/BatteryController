/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "can/services/Nissan/HappyPoller.hpp"
#include "can/FrameSink.hpp"
#include "can/StandardDataFrame.hpp"
#include "can/messages/Nissan/Ids.hpp"

namespace can {
namespace services {
namespace Nissan {

using namespace can::messages::Nissan;

HappyPoller::HappyPoller(FrameSink& sender, core::Timer& timer):
    m_sender(sender),
    m_timer(timer),
    m_heartbeat_poll_callback(*this, &HappyPoller::poll),
    m_heartbeat_counter(3)
{
   m_timer.registerPeriodicCallback(&m_heartbeat_poll_callback, 1000);
}

HappyPoller::~HappyPoller()
{
   m_timer.deregisterCallback(&m_heartbeat_poll_callback);
}

void HappyPoller::poll()
{
   m_sender.sink(StandardDataFrame(ID_LBC_HEARTBEAT_11A, m_heartbeat_11A[m_heartbeat_counter]));
   m_heartbeat_counter = (m_heartbeat_counter + 1) % 4;
}

void HappyPoller::received(const DataFrame& frame)
{
//   if (frame.id() != ID_LBC_DATA_REPLY) return;
   if (frame.size() != 8) return;
//   if (frame.data()[0] != 0x10) return;
//
//   const uint8_t request[8] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//   m_sender.sink(StandardDataFrame(ID_LBC_DATA_REQUEST, request));
}


}
}
}
