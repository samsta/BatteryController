/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CONTACTOR_NISSAN_LEAFCONTACTOR_HPP
#define _CONTACTOR_NISSAN_LEAFCONTACTOR_HPP

#include "contactor/Contactor.hpp"
#include "core/Callback.hpp"
#include "core/OutputPin.hpp"
#include "core/Timer.hpp"
#include "can/FrameSink.hpp"
#include "can/StandardDataFrame.hpp"
#include "logging/stream.hpp"
#include "can/messages/Nissan/Ids.hpp"

using namespace can::messages::Nissan;

namespace contactor {
namespace Nissan {

class LeafContactor: public Contactor
{
public:
   LeafContactor(core::Timer& timer,
                 core::OutputPin& positive_relay,
                 core::OutputPin& negative_relay,
                 core::OutputPin& indicator,
                 logging::ostream* log);

   ~LeafContactor();

   virtual void setSafeToOperate(bool);
   virtual bool isSafeToOperate() const;
   virtual bool isClosed() const;
   virtual void close();
   virtual void open();

private:
   enum State {
      OPEN,
      CLOSING,
      CLOSED
   };

   void openBothRelays();
   void updateRelays();
   void closeNegativeRelay();
   void closePositiveRelay();

   core::Timer&      m_timer;
   core::OutputPin&  m_positive_relay;
   core::OutputPin&  m_negative_relay;
   core::OutputPin&  m_indicator;
   logging::ostream* m_log;

   bool  m_safe_to_operate;
   State m_requested_state;
   State m_state;

   core::Callback<LeafContactor> m_delayed_close;
};

//---------------------------------------------------------------------------------------------------
class TeensyRelay: public Contactor
{
public:
   TeensyRelay(can::FrameSink& sender, uint32_t canid);
   ~TeensyRelay();

   virtual void setSafeToOperate(bool);
   virtual bool isSafeToOperate() const;
   virtual bool isClosed() const;
   virtual void close();
   virtual void open();

private:
   enum State {
      OPEN,
      CLOSED
   };

   void updateRelay();

   can::FrameSink&   m_sender;
   uint32_t m_canid;
   bool  m_safe_to_operate;
   State m_requested_state;
   State m_state;

   const uint8_t m_off_msg[8] = { 0x55, 0x55, 0x00, 0x00, 0x00, 0x00 };
   const uint8_t m_on_msg[8] = { 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00 };
};



}
}

#endif // _CONTACTOR_NISSAN_LEAFCONTACTOR_HPP
