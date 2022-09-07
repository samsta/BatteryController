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
      m_contactor_status(pow(2,6)-1)
//      ,
//      m_all_contactor()
{
}

LeafMultiPack::~LeafMultiPack()
{
}

//contactor::Contactor& LeafMultiPack::getContactor()
//{
//  return m_all_contactor;
//}

uint32_t LeafMultiPack::getContactorStatus() const
{
	return m_contactor_status;
}

void LeafMultiPack::setSafeToOperate(bool is_safe)
{
   // if (m_log)
   // {
   //    *m_log << color::red << ">>> contactor is " << (is_safe ? "safe" : "unsafe") << " to operate" << color::reset << std::endl;
   // }
   m_safe_to_operate = is_safe;
   updateRelays();
}

bool LeafMultiPack::isSafeToOperate() const
{
   return m_safe_to_operate;
}

bool LeafMultiPack::isClosed() const
{
   return m_state == CLOSED;
}

void LeafMultiPack::close()
{
   m_requested_state = CLOSED;
   updateRelays();
}

void LeafMultiPack::open()
{
   m_requested_state = OPEN;
   updateRelays();
}

void LeafMultiPack::updateRelays()
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

float LeafMultiPack::getVoltage() const
{
   return m_voltage;
}

float LeafMultiPack::getCurrent() const
{
   return m_current;
}

float LeafMultiPack::getTemperature() const
{
   return m_average_temperature;
}

float LeafMultiPack::getSocPercent() const
{
   return m_soc_percent;
}

float LeafMultiPack::getSohPercent() const
{
   return m_soh_percent;
}

float LeafMultiPack::getEnergyRemainingKwh() const
{
   return m_energy_remaining_kwh;
}

float LeafMultiPack::getCapacityKwh() const
{
   return m_capacity_kwh;
}

uint32_t LeafMultiPack::getSystemVersion() const
{
   return 1; // TODO - come up with a versioning scheme
}

uint32_t LeafMultiPack::getSerialNumber() const
{
   return 1; // TODO - do we configure this, or can we get it from the battery?
}

float LeafMultiPack::getNominalCapacityKwh() const
{
   return 0; // NOMINAL_CAPACITY_KWH; // TODO - configurable
}

unsigned LeafMultiPack::getNumberOfModules() const
{
   return 0;// NUM_MODULES; // TODO - configurable
}

uint32_t LeafMultiPack::getManufacturingDateUnixTime() const
{
   return 0; // TODO - where do we get this from?
}

const char* LeafMultiPack::getManufacturerName() const
{
   return "TIML";
}

const char* LeafMultiPack::getBatteryName() const
{
   return "LeafG2";
}

float LeafMultiPack::getMaxChargeVoltage() const
{
   return 0;// CRITICALLY_HIGH_VOLTAGE * NUM_CELLS;
}

float LeafMultiPack::getMinDischargeVoltage() const
{
   return 0;//CRITICALLY_LOW_VOLTAGE * NUM_CELLS;
}

float LeafMultiPack::getChargeCurrentLimit() const
{
   //return m_charge_cur_fac_by_voltage * m_cur_fac_by_temperature * NOMINAL_CURRENT_LIMIT;
   return m_charge_current_limit;
}

float LeafMultiPack::getDischargeCurrentLimit() const
{
   //return m_discharge_cur_fac_by_voltage * m_cur_fac_by_temperature * NOMINAL_CURRENT_LIMIT;
   return m_discharge_current_limit;
}






}
}



