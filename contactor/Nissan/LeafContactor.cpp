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

LeafContactor::LeafContactor(
   Timer& timer,
   OutputPin& positive_relay,
   OutputPin& negative_relay,
   OutputPin& indicator,
   logging::ostream* log):
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

void LeafContactor::setSafeToOperate(bool is_safe)
{
   if (m_log)
   {
      *m_log << color::red << ">>> contactor is " << (is_safe ? "safe" : "unsafe") << " to operate" << color::reset << std::endl;
   }
   m_safe_to_operate = is_safe;
   updateRelays();
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
      *m_log << color::bright_red << ">>>> contactor opened" << color::reset << std::endl;
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
      *m_log << color::bright_red << ">>>> contactor closing..." << color::reset << std::endl;
   }

   m_timer.schedule(&m_delayed_close, DELAY_CLOSE_MS);
   m_negative_relay.set(OutputPin::LOW);
   m_indicator.set(OutputPin::HIGH);
}

void LeafContactor::closePositiveRelay()
{
   m_positive_relay.set(OutputPin::LOW);

   if (m_log)
   {
      *m_log << color::bright_red <<  ">>>> contactor closed" << color::reset << std::endl;
   }
   m_state = CLOSED;
}


}
}