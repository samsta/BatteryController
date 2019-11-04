/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "can/services/Nissan/GroupPoller.hpp"
#include "can/FrameSink.hpp"
#include "can/StandardDataFrame.hpp"
#include "can/messages/Nissan/Ids.hpp"

namespace can {
namespace services {
namespace Nissan {

using namespace can::messages::Nissan;

GroupPoller::GroupPoller(FrameSink& sender):
    m_sender(sender),
    m_poll_ix(0),
    m_poll_groups{
      GROUP_BATTERY_STATE,
      GROUP_CELL_VOLTAGES,
      GROUP_CELL_VOLTAGE_RANGE,
      GROUP_PACK_TEMPERATURES}
{
}

void GroupPoller::poll()
{
   const uint8_t request[8] = {0x02, 0x21, m_poll_groups[m_poll_ix], 0x00, 0x00, 0x00, 0x00, 0x00};
   m_sender.sink(StandardDataFrame(ID_LBC_DATA_REQUEST, request));
   m_poll_ix = (m_poll_ix + 1) % sizeof(m_poll_groups);
}

void GroupPoller::received(const DataFrame& frame)
{
   if (frame.id() != ID_LBC_DATA_REPLY) return;
   if (frame.size() != 8) return;
   if (frame.data()[0] != 0x10) return;

   const uint8_t request[8] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   m_sender.sink(StandardDataFrame(ID_LBC_DATA_REQUEST, request));
}

}
}
}
