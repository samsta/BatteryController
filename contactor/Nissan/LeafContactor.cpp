/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "LeafContactor.hpp"
#include "logging/colors.hpp"

namespace color = logging::color::ansi;
using namespace core;

namespace contactor {
namespace Nissan {
namespace {
const unsigned DELAY_CLOSE_MS = 3000;
const unsigned DELAY_OPEN_MS = 5000;
}

//---------------------------------------------------------------------------------------------------
LeafContactor::LeafContactor(
   Timer& timer,
   OutputPin& positive_relay,
   OutputPin& negative_relay,
   OutputPin& pre_charge_relay,
   logging::Logger* log):
         m_timer(timer),
         m_positive_relay(positive_relay),
         m_negative_relay(negative_relay),
         m_pre_charge_relay(pre_charge_relay),
         m_log(log),
         m_safe_to_operate(false),
         m_requested_state(OPEN),
         m_state(OPEN),
         m_delayed_close(*this, &LeafContactor::closePositiveRelay),
         m_delayed_open(*this, &LeafContactor::openPreChargeRelay)
{
   openBothRelays();
}

LeafContactor::~LeafContactor()
{
   if (m_log)
   {
      std::ostringstream ss;
      ss << "LeafContactor: opening contractors at destruction";
      m_log->info(ss);
   }
   openBothRelays();
}

void LeafContactor::setSafeToOperate(bool is_safe)
{
   if (m_log)
   {
      std::ostringstream ss;
      ss << "LeafContactor is " << (is_safe ? "safe" : "unsafe") << " to operate";
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
   m_pre_charge_relay.set(OutputPin::HIGH);

   if (m_log)
   {
      std::ostringstream ss;
      ss <<  "LeafContactor: contactor opened";
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
      ss << "LeafContactor: closing... (neg and precharge CLOSED)";
      m_log->info(ss);
   }
   m_negative_relay.set(OutputPin::LOW);
   // also close the precharge relay
   m_pre_charge_relay.set(OutputPin::LOW);

   // the next line delays by DELAY_CLOSE_MS then calls m_delayed_close(callback), m_delayed_close is 
   // intialized in the class constructor to call closePositiveRelay... clear as mud.
   // same system for opening the pre-charge relay after DELAY_OPEN_MS
   m_timer.schedule(&m_delayed_close, DELAY_CLOSE_MS);
   m_timer.schedule(&m_delayed_open, DELAY_OPEN_MS);
}

void LeafContactor::closePositiveRelay()
{
   m_positive_relay.set(OutputPin::LOW);

   if (m_log)
   {
      std::ostringstream ss;
      ss << "LeafContactor: contactor CLOSED";
      m_log->info(ss);
   }
   m_state = CLOSED;
}

void LeafContactor::openPreChargeRelay()
{
   m_pre_charge_relay.set(OutputPin::HIGH);

   if (m_log)
   {
      std::ostringstream ss;
      ss << "LeafContactor: pre-charge OPEN";
      m_log->info(ss);
   }
}

//---------------------------------------------------------------------------------------------------
TeensyShuntCtrl::TeensyShuntCtrl(char *packname, can::FrameSink& sender, uint32_t canid, logging::Logger* log):
         m_pack_name(packname),
         m_sender(sender),
         m_canid(canid),
         m_log(log),
         m_safe_to_operate(true),
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
      std::ostringstream ss;
      ss << "ShuntCtrl: " << m_pack_name << ": shunt de-energized";
      if (m_log) m_log->info(ss, __FILENAME__,__LINE__);
}

void TeensyShuntCtrl::open()
{
      m_sender.sink(can::StandardDataFrame(m_canid, m_shunt_triggered_msg));
      m_state = TRIGGERED;
      std::ostringstream ss;
      ss << "ShuntCtrl: " << m_pack_name << ": SHUNT TRIGGERED";
      if (m_log) m_log->alarm(ss, __FILENAME__,__LINE__);
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
TeensyRelay::TeensyRelay(char *packname, can::FrameSink& sender, uint32_t canid, logging::Logger* log):
         m_pack_name(packname),
         m_sender(sender),
         m_canid(canid),
         m_log(log),
         m_state(DE_ENERGIZED)
{
   // open (de-energize) relay on startup
   open();
}

TeensyRelay::~TeensyRelay()
{
   // open (de-energize) relay on shutdown
   open();
}

void TeensyRelay::setState(enum TeensyRelay::State state)
{
   m_state = state;
   if (m_state == ENERGIZED)
   {
      close();
      std::ostringstream ss;
      ss << "TeensyRelay: " << m_pack_name << ": CLOSED (energized)";
      if (m_log) m_log->info(ss, __FILENAME__,__LINE__);
   }
   else
   {
      open();
      std::ostringstream ss;
      ss << "TeensyRelay: " << m_pack_name << ": OPEN (de-energized)";
      if (m_log) m_log->info(ss, __FILENAME__,__LINE__);
   }
}

bool TeensyRelay::isEnergized() const
{
   return m_state == ENERGIZED;
}

void TeensyRelay::close()
{
      m_sender.sink(can::StandardDataFrame(m_canid, m_close_msg));
      m_state = ENERGIZED;
}

void TeensyRelay::open()
{
      m_sender.sink(can::StandardDataFrame(m_canid, m_open_msg));
      m_state = DE_ENERGIZED;
}


}
}
