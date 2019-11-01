/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_SERVICES_NISSAN_GROUPPOLLER
#define _CAN_SERVICES_NISSAN_GROUPPOLLER

#include <stdint.h>

namespace can {

class FrameSink;
class DataFrame;

namespace services {
namespace Nissan {

class GroupPoller
{
public:
   GroupPoller(FrameSink& sender);

   void poll();
   void received(const DataFrame&);

private:
   FrameSink& m_sender;
   unsigned m_poll_ix;
   uint8_t m_poll_groups[3];
};

}
}
}

#endif //  _CAN_SERVICES_NISSAN_GROUPPOLLER
