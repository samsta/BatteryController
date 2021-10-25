/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "MessageFactory.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"
#include "logging/colors.hpp"
#include <stdlib.h>

using namespace can::messages::Nissan;
namespace color = logging::color::ansi;

#ifndef HAS_NEW
// weirdly, arduino doesn't have placement new
inline void *operator new(size_t, void *buf) { return buf; }
#endif

namespace can {
namespace services {
namespace Nissan {
namespace {

const Message* decode(const can::DataFrame& f, void* mem)
{
   Message* msg;

   msg = new(mem) BatteryStatus(f);
   if (msg->valid()) return msg;

   msg = new(mem) CellVoltages(f);
   if (msg->valid()) return msg;

   msg = new(mem) PackTemperatures(f);
   if (msg->valid()) return msg;

   msg = new(mem) CellVoltageRange(f);
   if (msg->valid()) return msg;

   msg = new(mem) BatteryState(f);
   if (msg->valid()) return msg;

   return NULL;
}

}


MessageFactory::MessageFactory(messages::Nissan::MessageSink& sink, logging::ostream* log):
   m_sink(sink),
   m_log(log)
{
}

void MessageFactory::sink(const can::DataFrame& f)
{
   const Message* msg = decode(f, m_message_memory);

   if (msg == nullptr) return;

   if (m_log)
   {
      *m_log << color::bright_blue << "<BAT IN> " << *msg << color::reset << std::endl;
   }
   m_sink.sink(*msg);
}


}
}
}
