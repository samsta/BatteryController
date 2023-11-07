/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_SERVICES_NISSAN_MESSAGEFACTORY_HPP_
#define _CAN_SERVICES_NISSAN_MESSAGEFACTORY_HPP_

#include "can/FrameSink.hpp"
#include "can/messages/Nissan/Message.hpp"
#include "logging/logging.hpp"

namespace can {
namespace services {
namespace Nissan {

class MessageFactory: public can::FrameSink
{
public:
   MessageFactory(messages::Nissan::MessageSink& sink, logging::Logger* log, std::string name);

   virtual void sink(const DataFrame& frame);

private:
   messages::Nissan::MessageSink& m_sink;
   std::string m_name;
   logging::Logger* m_log;
   uint8_t m_message_memory[1024];
};

}
}
}

#endif /* _CAN_SERVICES_NISSAN_MESSAGEFACTORY_HPP_ */
