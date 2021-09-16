#ifndef INVERTER_TSUN_TSOL_H50K_HPP_
#define INVERTER_TSUN_TSOL_H50K_HPP_

#include "can/FrameSink.hpp"
#include "contactor/Contactor.hpp"
#include "core/Timer.hpp"
#include "inverter/Inverter.hpp"
#include "monitor/Monitor.hpp"

namespace can {
namespace messages {
class Message;
namespace TSUN {
class InverterInfoRequest;
}
}
}
namespace inverter {
namespace TSUN {

class TSOL_H50K: public Inverter
{
public:
   TSOL_H50K(can::FrameSink& sender,
                   core::Timer& timer,
                   monitor::Monitor& monitor,
                   contactor::Contactor& contactor);
   ~TSOL_H50K();

   void sink(const can::messages::Message&);

private:
   void process(const can::messages::TSUN::InverterInfoRequest&);
   void periodicCallback();

   can::FrameSink&       m_sender;
   core::Timer&          m_timer;
   monitor::Monitor&     m_monitor;
   contactor::Contactor& m_contactor;
   core::Callback<TSOL_H50K> m_periodic_callback;
   unsigned              m_inverter_silent_counter;
};

}
}

#endif /* INVERTER_TSUN_TSOL_H50K_HPP_ */
