/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "MessageFactory.hpp"
#include "can/messages/Tesla/TSTemperatures.hpp"
#include "can/messages/Tesla/TSCellVoltages.hpp"
#include "can/messages/Tesla/TSModCellVoltages.hpp"
#include "can/messages/Tesla/TSModVoltTemp.hpp"
#include "can/messages/Tesla/TSBatteryStatus.hpp"
#include "can/messages/Tesla/TSCurrentEnergy.hpp"
#include "logging/colors.hpp"
#include <stdlib.h>

using namespace can::messages::Tesla;
namespace color = logging::color::ansi;

#ifndef HAS_NEW
// weirdly, arduino doesn't have placement new
inline void *operator new(size_t, void *buf) { return buf; }
#endif

namespace can {
namespace services {
namespace Tesla {
namespace {

const Message* decode(const can::DataFrame& f, void* mem)
{
   Message* msg;

   msg = new(mem) TSTemperatures(f);
   if (msg->valid()) return msg;

   msg = new(mem) TSVoltages(f);
   if (msg->valid()) return msg;

   msg = new(mem) TSBatteryStatus(f);
   if (msg->valid()) return msg;

   msg = new(mem) TSCurrentEnergy(f);
   if (msg->valid()) return msg;

   msg = new(mem) TSModCellVoltages(f);
   if (msg->valid()) return msg;

   msg = new(mem) TSModVoltTemp(f);
   if (msg->valid()) return msg;

   // msg = new(mem) PackTemperatures(f);
   // if (msg->valid()) return msg;

   // msg = new(mem) CellVoltageRange(f);
   // if (msg->valid()) return msg;

   // msg = new(mem) BatteryState(f);
   // if (msg->valid()) return msg;

   // msg = new(mem) BatteryPowerLimits(f);
   // if (msg->valid()) return msg;

   return NULL;
}

}


MessageFactory::MessageFactory(messages::Tesla::MessageSink& sink, logging::Logger* log, std::string name):
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
      if (m_log) m_log->debug(ss);
   }
   m_sink.sink(*msg);
}


}
}
}
