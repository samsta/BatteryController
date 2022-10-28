/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "TeensyRelay.hpp"
#include "can/StandardDataFrame.hpp"
#include "logging/colors.hpp"
#include <errno.h>
#include <iostream>
#include <linux/can/raw.h>
#include <linux/can.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdint.h>
#include <termios.h>

namespace core {

TeensyRelay::TeensyRelay(FrameSink& sender, uint32_t canid, uint8_t on_msg[8], uint8_t off_msg[8]):
   m_sender(sender),
   m_canid(canid),
   m_on_msg(on_msg),
   m_off_msg(off_msg)
{
}

TeensyRelay::~TeensyRelay()
{
   // do we want to turn the relay off on distruction?
}

void TeensyRelay::setRelayState(bool newstate)
{
   m_relay_state = newstate;
   if (m_relay_state)
   {
      m_sender.sink(StandardDataFrame(m_canid, m_on_msg));
   }
   else
   {
      m_sender.sink(StandardDataFrame(m_canid, m_off_msg));
   }
}

bool TeensyRelay::getRelayState()
{
   return m_relay_state;
}

} // namespace core
