/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "MessageFactory.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"
#include "can/messages/Nissan/BatteryPowerLimits.hpp"
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

   msg = new(mem) BatteryPowerLimits(f);
   if (msg->valid()) return msg;

   return NULL;
}

}


MessageFactory::MessageFactory(messages::Nissan::MessageSink& sink, logging::Logger* log, std::string name):
   m_sink(sink),
   m_log(log),
   m_name(name)
{
}

void MessageFactory::sink(const can::DataFrame& f)
{
   const Message* msg = decode(f, m_message_memory);
   std::ostringstream ss;

   if (msg == nullptr) return;

   if (m_log)
   {
      ss << "<BAT IN: " << m_name << "> " << *msg;
      m_log->debug(ss);
   }
   m_sink.sink(*msg);
}


}
}
}
