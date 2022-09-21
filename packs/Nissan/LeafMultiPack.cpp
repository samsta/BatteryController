/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafMultiPack.hpp>

namespace packs {
namespace Nissan {

LeafMultiPack::LeafMultiPack(unsigned int num_packs,
                     monitor::Monitor* monitor1,
                     contactor::Contactor* contactor1,
                     // monitor::Monitor& monitor2,
                     // contactor::Contactor& contactor2,
                     // monitor::Monitor& monitor3,
                     // contactor::Contactor& contactor3,
                     logging::ostream* log):
      m_num_packs(num_packs),
      m_1monitor(monitor1),
      m_1contactor(contactor1),
      // m_2monitor(monitor2),
      // m_2contactor(contactor2),
      // m_3monitor(monitor3),
      // m_3contactor(contactor3),
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
      m_safe_to_operate(false),
      m_requested_state(OPEN),
      m_state(OPEN)
{
}

LeafMultiPack::~LeafMultiPack()
{
}

uint32_t LeafMultiPack::getFailsafeStatus() const
{
	return m_1monitor->getFailsafeStatus();
}

uint32_t LeafMultiPack::getContactorStatus() const
{
   return (m_1monitor->getContactorStatus());
   // return (m_1monitor->getContactorStatus() | m_2monitor.getContactorStatus());
}

// disable compiler warning for this method
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void LeafMultiPack::setSafeToOperate(bool is_safe)
{
   // should never be called!
   printf("\nLeafMultiPack::setSafeToOperate called: THIS SHOULD NEVER HAPPEN!\n");
}
#pragma GCC diagnostic pop

bool LeafMultiPack::isSafeToOperate() const
{
   return m_safe_to_operate;
}

bool LeafMultiPack::isClosed() const
{
   return (m_1monitor->getContactorStatus() == CLOSED);
   // return m_state == CLOSED;
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
   m_safe_to_operate = m_1contactor->isSafeToOperate(); // & m_2contactor.isSafeToOperate();
   if (m_safe_to_operate && m_requested_state == CLOSED)
   {
      if (m_state == OPEN)
      {
         m_1contactor->close();
         // m_2contactor.close();
      }
   }
   else if (not m_safe_to_operate or m_requested_state == OPEN)
   {
      if (m_state != OPEN)
      {
         m_1contactor->open();
         // m_2contactor.open();
      }
   }
}

float LeafMultiPack::getVoltage() const
{
   return m_1monitor->getVoltage();
   // return (m_1monitor->getVoltage() + m_2monitor.getVoltage())/ 2.0;
}

float LeafMultiPack::getCurrent() const
{
   return m_1monitor->getCurrent();
}

float LeafMultiPack::getTemperature() const
{
   return  m_1monitor->getTemperature();
}

float LeafMultiPack::getSocPercent() const
{
   // printf("LeafMultiPack::getSocPercent()");
   return m_1monitor->getSocPercent();
}

float LeafMultiPack::getSohPercent() const
{
   // printf("LeafMultiPack::getSohPercent()");
   return m_1monitor->getSohPercent();
}

float LeafMultiPack::getEnergyRemainingKwh() const
{
   return m_1monitor->getEnergyRemainingKwh();
}

float LeafMultiPack::getCapacityKwh() const
{
   return m_1monitor->getCapacityKwh();
}

uint32_t LeafMultiPack::getSystemVersion() const
{
   return 2; // TODO - come up with a versioning scheme
}

uint32_t LeafMultiPack::getSerialNumber() const
{
   return 2; // TODO - do we configure this, or can we get it from the battery?
}

float LeafMultiPack::getNominalCapacityKwh() const
{
   return m_1monitor->getNominalCapacityKwh(); // NOMINAL_CAPACITY_KWH; // TODO - configurable
}

unsigned LeafMultiPack::getNumberOfModules() const
{
   return m_1monitor->getNumberOfModules(); // NUM_MODULES; // TODO - configurable
}

uint32_t LeafMultiPack::getManufacturingDateUnixTime() const
{
   return 2; // TODO - where do we get this from?
}

const char* LeafMultiPack::getManufacturerName() const
{
   // printf("LeafMultiPack::getManufacturerName()");
   return "TIML-LMP";
}

const char* LeafMultiPack::getBatteryName() const
{
   return "LeafG2-LMP";
}

float LeafMultiPack::getMaxChargeVoltage() const
{
   return m_1monitor->getMaxChargeVoltage(); 
}

float LeafMultiPack::getMinDischargeVoltage() const
{
   return m_1monitor->getMinDischargeVoltage(); 
}

float LeafMultiPack::getChargeCurrentLimit() const
{
   return m_1monitor->getChargeCurrentLimit();
}

float LeafMultiPack::getDischargeCurrentLimit() const
{
   return m_1monitor->getDischargeCurrentLimit();
}

}
}



