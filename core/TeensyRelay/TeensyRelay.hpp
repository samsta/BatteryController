/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CORE_TEENSYRELAY_TEENSYRELAY_HPP_
#define CORE_TEENSYRELAY_TEENSYRELAY_HPP_

#include "can/FrameSink.hpp"

namespace core
{

class TeensyRelay
{
public:
   TeensyRelay(FrameSink& sender, uint32_t canid, uint8_t on_msg[8], uint8_t off_msg[8]);
   ~TeensyRelay();

   bool getRelayState();
   void setRelayState(bool newstate);

private:
   FrameSink&   m_sender;
   uint32_t m_canid;
   uint8_t m_on_msg[8];
   uint8_t m_off_msgp[8];
   bool m_relay_state;
   
};

}
#endif /* CORE_TEENSYRELAY_TEENSYRELAY_HPP_ */
