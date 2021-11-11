/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CAN_SERVICES_NISSAN_HAPPYPOLLER_HPP_
#define CAN_SERVICES_NISSAN_HAPPYPOLLER_HPP_

#include "core/Timer.hpp"
#include <stdint.h>

namespace can {

class FrameSink;
class DataFrame;

namespace services {
namespace Nissan {

class HappyPoller
{
public:
   HappyPoller(FrameSink& sender, core::Timer& timer);
   ~HappyPoller();

   void received(const DataFrame&);

private:
   void poll();

   FrameSink&   m_sender;
   core::Timer& m_timer;

   core::Callback<HappyPoller> m_heartbeat_poll_callback;

   uint8_t m_heartbeat_counter;
   uint8_t m_heartbeat_11A[4][8] = {{ 0x4E, 0x40, 0x04, 0xAA, 0xC0, 0x00, 0x00, 0xBD},
                              { 0x4E, 0x40, 0x04, 0x55, 0x00, 0x00, 0x01, 0xAB},
                              { 0x4E, 0x40, 0x04, 0x55, 0x40, 0x00, 0x02, 0x66},
                              { 0x4E, 0x40, 0x04, 0xAA, 0x80, 0x00, 0x03, 0x70}};
};


}
}
}

#endif /* CAN_SERVICES_NISSAN_HAPPYPOLLER_HPP_ */
