/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_SERVICES_NISSAN_GROUPPOLLER
#define _CAN_SERVICES_NISSAN_GROUPPOLLER

#include "core/Timer.hpp"
#include <stdint.h>

namespace can {

class FrameSink;
class DataFrame;

namespace services {
namespace Nissan {

class GroupPoller
{
public:
   GroupPoller(FrameSink& sender, core::Timer& timer);
   ~GroupPoller();

   void received(const DataFrame&);

private:
   void poll();

   FrameSink&   m_sender;
   core::Timer& m_timer;
   unsigned     m_poll_ix;
   uint8_t      m_poll_groups[4];

   core::Callback<GroupPoller> m_poll_callback;
};

}
}
}

#endif //  _CAN_SERVICES_NISSAN_GROUPPOLLER
