#ifndef INVERTER_SINEX_SE_PWS2_HPP_
#define INVERTER_SINEX_SE_PWS2_HPP_

#include "can/FrameSink.hpp"
#include "can/messages/Message.hpp"
#include "contactor/Contactor.hpp"
#include "core/Timer.hpp"
#include "inverter/Inverter.hpp"
#include "monitor/Monitor.hpp"
#include "logging/logging.hpp"

namespace can {
namespace messages {
namespace SINEX {
class InverterHeartbeat;
}
}
}
namespace inverter {
namespace SINEX {

class SE_PWS2: public Inverter, public can::messages::MessageSink
{
public:
   SE_PWS2(can::FrameSink& sender,
                   core::Timer& timer,
                   monitor::Monitor& monitor,
                   contactor::Contactor& contactor,
                   logging::Logger *log);
   ~SE_PWS2();

   virtual void sink(const can::messages::Message&);
   uint16_t getHeartbeatValue();
   bool getHbNonConsec();

private:
   void process(const can::messages::SINEX::InverterHeartbeat& command);
   void periodicCallback();

   can::FrameSink&       m_sender;
   core::Timer&          m_timer;
   monitor::Monitor&     m_monitor;
   contactor::Contactor& m_contactor;
   logging::Logger       *m_log;
   core::Callback<SE_PWS2> m_periodic_callback;
   uint16_t              m_heartbeat_count;
   bool                  m_first_heartbeat;
   unsigned              m_inverter_silent_counter;
   bool                  m_hb_non_consec;

};

}
}





#endif /* INVERTER_SINEX_SE_PWS2_HPP_ */
