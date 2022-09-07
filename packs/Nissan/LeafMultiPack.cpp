/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafMultiPack.hpp>

namespace packs {
namespace Nissan {

LeafMultiPack::LeafMultiPack(
                     monitor::Monitor& monitor1,
                     contactor::Contactor& contactor1,
                     monitor::Monitor& monitor2,
                     contactor::Contactor& contactor2,
                     logging::ostream* log):
      m_1monitor(monitor1),
      m_1contactor(contactor1),
      m_2monitor(monitor2),
      m_2contactor(contactor2),
      m_log(log),
      m_voltages_ok(false),
      m_temperatures_ok(false),
      m_everything_ok(false),
      m_soc_percent(NAN),
      m_soh_percent(NAN),
      m_energy_remaining_kwh(NAN),
      m_capacity_kwh(NAN),
      m_current(NAN),
      m_voltage(NAN),
      m_average_temperature(NAN),
      m_cur_fac_by_temperature(0),
      m_charge_cur_fac_by_voltage(0),
      m_discharge_cur_fac_by_voltage(0),
      m_discharge_power_limit(NAN),
      m_charge_power_limit(NAN),
      m_discharge_current_limit(0),
      m_charge_current_limit(0),
      m_contactor_status(pow(2,6)-1),
      m_all_contactor()
{
}

LeafMultiPack::~LeafMultiPack()
{
}

contactor::Contactor& LeafMultiPack::getContactor()
{
  return m_all_contactor;
}

uint32_t LeafMultiPack::getContactorStatus() const
{
	return m_contactor_status;
}

void LeafMultiPack::AllContactor::setSafeToOperate(bool is_safe)
{
   // if (m_log)
   // {
   //    *m_log << color::red << ">>> contactor is " << (is_safe ? "safe" : "unsafe") << " to operate" << color::reset << std::endl;
   // }
   m_safe_to_operate = is_safe;
   updateRelays();
}

bool LeafMultiPack::AllContactor::isSafeToOperate() const
{
   return m_safe_to_operate;
}

bool LeafMultiPack::AllContactor::isClosed() const
{
   return m_state == CLOSED;
}

void LeafMultiPack::AllContactor::close()
{
   m_requested_state = CLOSED;
   updateRelays();
}

void LeafMultiPack::AllContactor::open()
{
   m_requested_state = OPEN;
   updateRelays();
}

void LeafMultiPack::AllContactor::updateRelays()
{
   // if (m_safe_to_operate && m_requested_state == CLOSED)
   // {
   //    if (m_state == OPEN) closeNegativeRelay();
   // }
   // else if (not m_safe_to_operate or m_requested_state == OPEN)
   // {
   //    if (m_state != OPEN) openBothRelays();
   // }
}


}
}



