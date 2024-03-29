#ifndef INVERTER_TSUN_TSOL_H50K_HPP_
#define INVERTER_TSUN_TSOL_H50K_HPP_

#include "can/FrameSink.hpp"
#include "can/messages/Message.hpp"
#include "contactor/Contactor.hpp"
#include "core/Timer.hpp"
#include "inverter/Inverter.hpp"
#include "monitor/Monitor.hpp"
#include "logging/logging.hpp"

namespace can {
namespace messages {
namespace TSUN {
class InverterInfoRequest;
}
}
}
namespace inverter {
namespace TSUN {

class TSOL_H50K: public Inverter, public can::messages::MessageSink
{
public:
   TSOL_H50K(can::FrameSink& sender,
                   core::Timer& timer,
                   monitor::Monitor& monitor,
                   contactor::Contactor& contactor,
                   logging::Logger *log);
   ~TSOL_H50K();

   virtual void sink(const can::messages::Message&);

private:
   void process(const can::messages::TSUN::InverterInfoRequest& command);
   void periodicCallback();

   can::FrameSink&       m_sender;
   core::Timer&          m_timer;
   monitor::Monitor&     m_monitor;
   contactor::Contactor& m_contactor;
   logging::Logger       *m_log;
   core::Callback<TSOL_H50K> m_periodic_callback;
   unsigned              m_inverter_silent_counter;
};

}
}

#endif /* INVERTER_TSUN_TSOL_H50K_HPP_ */
