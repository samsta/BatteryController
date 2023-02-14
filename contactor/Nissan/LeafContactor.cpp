/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "LeafContactor.hpp"
#include "logging/colors.hpp"

namespace color = logging::color::ansi;
using namespace core;

namespace contactor {
namespace Nissan {
namespace {
const unsigned DELAY_CLOSE_MS = 3000;
}

//---------------------------------------------------------------------------------------------------
LeafContactor::LeafContactor(
   Timer& timer,
   OutputPin& positive_relay,
   OutputPin& negative_relay,
   OutputPin& indicator,
   logging::Logger* log):
         m_timer(timer),
         m_positive_relay(positive_relay),
         m_negative_relay(negative_relay),
         m_indicator(indicator),
         m_log(log),
         m_safe_to_operate(false),
         m_requested_state(OPEN),
         m_state(OPEN),
         m_delayed_close(*this, &LeafContactor::closePositiveRelay)
{
   openBothRelays();
}

LeafContactor::~LeafContactor()
{
   if (m_log)
   {
      std::ostringstream ss;
      ss << ">>> opening contractors at destruction";
      m_log->info(ss);
   }
   openBothRelays();
}

void LeafContactor::setSafeToOperate(bool is_safe)
{
   if (m_log)
   {
      std::ostringstream ss;
      ss << ">>> contactor is " << (is_safe ? "safe" : "unsafe") << " to operate";
      m_log->info(ss);
   }
   m_safe_to_operate = is_safe;
   updateRelays();
}

bool LeafContactor::isSafeToOperate() const
{
   return m_safe_to_operate;
}

bool LeafContactor::isClosed() const
{
   return m_state == CLOSED;
}

void LeafContactor::close()
{
   m_requested_state = CLOSED;
   updateRelays();
}

void LeafContactor::open()
{
   m_requested_state = OPEN;
   updateRelays();
}

void LeafContactor::openBothRelays()
{
   m_negative_relay.set(OutputPin::HIGH);
   m_positive_relay.set(OutputPin::HIGH);
   m_indicator.set(OutputPin::LOW);

   if (m_log)
   {
      std::ostringstream ss;
      ss <<  ">>>> contactor opened";
      m_log->info(ss);
   }
   m_state = OPEN;
}

void LeafContactor::updateRelays()
{
   if (m_safe_to_operate && m_requested_state == CLOSED)
   {
      if (m_state == OPEN) closeNegativeRelay();
   }
   else if (not m_safe_to_operate or m_requested_state == OPEN)
   {
      if (m_state != OPEN) openBothRelays();
   }
}

void LeafContactor::closeNegativeRelay()
{
   m_state = CLOSING;
   if (m_log)
   {
      std::ostringstream ss;
      ss << ">>>> contactor closing...";
      m_log->info(ss);
   }

   // the next line delays by DELAY_CLOSE_MS then calls m_delayed_close(callback), m_delayed_close is 
   // intialized in the class constructor to call closePositiveRelay... clear as mud.
   m_timer.schedule(&m_delayed_close, DELAY_CLOSE_MS);
   m_negative_relay.set(OutputPin::LOW);
   m_indicator.set(OutputPin::HIGH);
}

void LeafContactor::closePositiveRelay()
{
   m_positive_relay.set(OutputPin::LOW);

   if (m_log)
   {
      std::ostringstream ss;
      ss << ">>>> contactor closed";
      m_log->info(ss);
   }
   m_state = CLOSED;
}

//---------------------------------------------------------------------------------------------------
TeensyShuntCtrl::TeensyShuntCtrl(can::FrameSink& sender, uint32_t canid):
         m_sender(sender),
         m_canid(canid),
         m_safe_to_operate(false),
         m_state(NORMAL)
{
   // ensure shunt is closed at startup (relay de-engerized), shunt not triggered,
   // shunt has to be closed manually (my hand) if it has been triggered
   close();
}

TeensyShuntCtrl::~TeensyShuntCtrl()
{
   // close shunt on shutdown, see above
   close();
}

void TeensyShuntCtrl::setSafeToOperate(bool is_safe)
{
   m_safe_to_operate = is_safe;
   updateRelay();
}

bool TeensyShuntCtrl::isSafeToOperate() const
{
   return m_safe_to_operate;
}

bool TeensyShuntCtrl::isClosed() const
{
   return m_state == NORMAL;
}

void TeensyShuntCtrl::close()
{
      m_sender.sink(can::StandardDataFrame(m_canid, m_shunt_normal_msg));
      m_state = NORMAL;
}

void TeensyShuntCtrl::open()
{
      m_sender.sink(can::StandardDataFrame(m_canid, m_shunt_triggered_msg));
      m_state = TRIGGERED;
}

void TeensyShuntCtrl::updateRelay()
{
   if (m_safe_to_operate)
   {
      close();
   }
   else // if (not m_safe_to_operate)
   {
      open();
   }
}

//---------------------------------------------------------------------------------------------------
// TeensyRelay::TeensyRelay(can::FrameSink& sender, uint32_t canid):
//          m_sender(sender),
//          m_canid(canid),
//          m_safe_to_operate(true),
//          m_state(OPEN)
// {
//    // open (de-energize) relay on startup
//    open();
// }

// TeensyRelay::~TeensyRelay()
// {
//    // open (de-energize) relay on shutdown
//    open();
// }

// void TeensyRelay::setSafeToOperate(bool is_safe)
// {
//    m_safe_to_operate = is_safe;
//    updateRelay();
// }

// bool TeensyRelay::isSafeToOperate() const
// {
//    return m_safe_to_operate;
// }

// bool TeensyRelay::isClosed() const
// {
//    return m_state == CLOSED;
// }

// void TeensyRelay::close()
// {
//       m_sender.sink(can::StandardDataFrame(m_canid, m_close_msg));
//       m_state = CLOSED;
// }

// void TeensyRelay::open()
// {
//       m_sender.sink(can::StandardDataFrame(m_canid, m_open_msg));
//       m_state = OPEN;
// }

// void TeensyRelay::updateRelay()
// {
//    if (m_safe_to_operate)
//    {
//       close();
//    }
//    else // if (not m_safe_to_operate)
//    {
//       open();
//    }
// }

}
}
