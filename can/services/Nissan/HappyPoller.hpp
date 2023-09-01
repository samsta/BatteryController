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
   const uint8_t m_vcm_diag_50b[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00 };
   uint8_t m_hcm_clock_50c_counter;
   uint8_t m_0x55_crc_50c[4] = { 0xA6, 0x31, 0x0D, 0x9A };
   uint8_t m_0xAA_crc_50c[4] = { 0xC8, 0x5F, 0x63, 0xF4 };
   uint8_t m_hcm_clock_50c[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

   uint8_t m_vcm_1F2[4][8] = {{ 0x10, 0x64, 0x00, 0xB4, 0x00, 0x1E, 0x00, 0x83},
                              { 0x10, 0x64, 0x00, 0xB4, 0x00, 0x1E, 0x01, 0x84},
                              { 0x10, 0x64, 0x00, 0xB4, 0x00, 0x1E, 0x02, 0x85},
                              { 0x10, 0x64, 0x00, 0xB4, 0x00, 0x1E, 0x03, 0x86}};

   uint8_t m_vcm_1D4[4][8] = {{ 0x6E, 0x6E, 0x00, 0x00, 0x07, 0x46, 0x21, 0xC0},
                              { 0x6E, 0x6E, 0x00, 0x00, 0x47, 0x46, 0x21, 0x07},
                              { 0x6E, 0x6E, 0x00, 0x00, 0x87, 0x46, 0x21, 0xCB},
                              { 0x6E, 0x6E, 0x00, 0x00, 0xC7, 0x46, 0x21, 0x0C}};

};

}
}
}

#endif /* CAN_SERVICES_NISSAN_HAPPYPOLLER_HPP_ */
