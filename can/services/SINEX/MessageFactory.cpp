/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "MessageFactory.hpp"
#include "can/messages/SINEX/InverterHeartbeat.hpp"
#include "logging/colors.hpp"
#include <stdlib.h>

using namespace can::messages::SINEX;
using can::messages::Message;
namespace color = logging::color::ansi;

#ifndef HAS_NEW
// weirdly, arduino doesn't have placement new
inline void *operator new(size_t, void *buf) { return buf; }
#endif

namespace can {
namespace services {
namespace SINEX {
namespace {

const Message* decode(const can::DataFrame& f, void* mem)
{
   Message* msg;

   msg = new(mem) InverterHeartbeat(f);
   if (msg->valid()) return msg;

   return NULL;
}
}

MessageFactory::MessageFactory(messages::MessageSink& sink, logging::Logger* log):
   m_sink(sink),
   m_log(log)
{
}

void MessageFactory::sink(const can::DataFrame& f)
{
   const Message* msg = decode(f, m_message_memory);
   std::ostringstream ss;

   if (msg == nullptr) return; // TODO should this be an error?

   if (m_log && m_log->isdebug()) {
      ss << "<INV IN>  " << *msg;
      m_log->debug(ss);
   }
   m_sink.sink(*msg);

}

}
}
}
