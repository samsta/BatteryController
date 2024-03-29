/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "MessageFactory.hpp"
#include "can/messages/SMA/InverterCommand.hpp"
#include "can/messages/SMA/InverterIdentity.hpp"
#include "can/messages/SMA/InverterManufacturer.hpp"
#include "logging/colors.hpp"
#include <stdlib.h>

using namespace can::messages::SMA;
using can::messages::Message;
namespace color = logging::color::ansi;

#ifndef HAS_NEW
// weirdly, arduino doesn't have placement new
inline void *operator new(size_t, void *buf) { return buf; }
#endif

namespace can {
namespace services {
namespace SMA {
namespace {

const Message* decode(const can::DataFrame& f, void* mem)
{
   Message* msg;

   msg = new(mem) InverterCommand(f);
   if (msg->valid()) return msg;

   msg = new(mem) InverterIdentity(f);
   if (msg->valid()) return msg;

   msg = new(mem) InverterManufacturer(f);
   if (msg->valid()) return msg;

   return NULL;
}

}


MessageFactory::MessageFactory(messages::MessageSink& sink, logging::ostream* log):
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
      *m_log << color::bright_green << "<INV IN>  " << *msg << color::reset << std::endl;
   }
   m_sink.sink(*msg);
}


}
}
}
