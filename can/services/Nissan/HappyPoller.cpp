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
    m_heartbeat_counter(3),
    m_hcm_clock_50c_counter(0)
{
   // was 10ms as on a real Leaf but that's unnecessary for our application
   // I have set this to 200... must be 200 to pass the 'tests' test_HappyPoller.cpp
   // todo I do not know at this time if 200 will satisfy the real battery
   m_timer.registerPeriodicCallback(&m_heartbeat_poll_callback, 10 * 60 * 1000); // set to 10 minutes
}

HappyPoller::~HappyPoller()
{
   m_timer.deregisterCallback(&m_heartbeat_poll_callback);
}

void HappyPoller::poll()
{
   //todo: if necessary, delay sending of message for a bit at startup
   // m_sender.sink(StandardDataFrame(ID_LBC_HEARTBEAT_11A, m_heartbeat_11A[m_heartbeat_counter]));
   m_heartbeat_counter = (m_heartbeat_counter + 1) % 4;

   // 1F2 uses 11A counter
   // m_sender.sink(StandardDataFrame(ID_LBC_VCM_1F2, m_vcm_1F2[m_heartbeat_counter]));

   // 1D4 uses 11A counter
   // m_sender.sink(StandardDataFrame(ID_LBC_VCM_1D4, m_vcm_1D4[m_heartbeat_counter]));

}

void HappyPoller::received(const DataFrame& frame)
{
   if (frame.id() != ID_LBC_STATUS_55B) return;
   if (frame.size() != 8) return;

   // m_hcm_clock_50c[3] = m_hcm_clock_50c_counter;

   // switch(frame.data()[2])
   // {
   // case 0x55:
   //    m_hcm_clock_50c[4] = 0xB2;
   //    m_hcm_clock_50c[5] = m_0x55_crc_50c[m_hcm_clock_50c_counter];
   //    break;

   // case 0xAA:
   //    m_hcm_clock_50c[4] = 0x5D;
   //    m_hcm_clock_50c[5] = m_0xAA_crc_50c[m_hcm_clock_50c_counter];
   //    break;

   // default:
   //    m_hcm_clock_50c[4] = 0xFF;
   //    m_hcm_clock_50c[5] = 0xFF;
   // }
   // m_sender.sink(StandardDataFrame(ID_LBC_HCM_CLOCK_50C, m_hcm_clock_50c, sizeof(m_hcm_clock_50c)));

   m_sender.sink(StandardDataFrame(ID_LBC_VCM_DIAG_50B, m_vcm_diag_50b));

   // m_hcm_clock_50c_counter = (m_hcm_clock_50c_counter + 1) % 4;

}


}
}
}
