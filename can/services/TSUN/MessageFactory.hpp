/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_SERVICES_TSUN_MESSAGEFACTORY_HPP_
#define _CAN_SERVICES_TSUN_MESSAGEFACTORY_HPP_

#include "can/FrameSink.hpp"
#include "can/messages/Message.hpp"
#include "logging/logging.hpp"

namespace can {
namespace services {
namespace TSUN {

class MessageFactory: public can::FrameSink
{
public:
   MessageFactory(messages::MessageSink& sink, CPlusPlusLogging::Logger* log);

   virtual void sink(const DataFrame& frame);

private:
   messages::MessageSink& m_sink;
   CPlusPlusLogging::Logger* m_log;
   uint8_t m_message_memory[1024];
};

}
}
}

#endif /* _CAN_SERVICES_TSUN_MESSAGEFACTORY_HPP_ */
