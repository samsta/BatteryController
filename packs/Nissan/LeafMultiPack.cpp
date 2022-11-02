/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafMultiPack.hpp>

namespace packs {
namespace Nissan {

LeafMultiPack::LeafMultiPack(
            std::vector<monitor::Monitor*> vmonitor,
            std::vector<contactor::Contactor*> vsafetyrelay,
            core::Timer& timer,
            core::OutputPin& positive_relay,
            core::OutputPin& negative_relay,
            core::OutputPin& indicator,
            logging::ostream* log):

      m_vmonitor(vmonitor),
      m_vsafety_relay(vsafetyrelay),
      m_timer(timer),
      m_main_contactor(
         timer,
         positive_relay,
         negative_relay,
         indicator,
         log),
      m_log(log),
      m_periodic_callback(*this, &LeafMultiPack::periodicCallback),
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
      m_discharge_power_limit(NAN),
      m_charge_power_limit(NAN),
      m_discharge_current_limit(0),
      m_charge_current_limit(0)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, 1000);
}

LeafMultiPack::~LeafMultiPack()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void LeafMultiPack::periodicCallback()
{
   printf("PERIODIC CALLBACK\n\r");
}


uint32_t LeafMultiPack::getFailsafeStatus() const
{
	return m_vmonitor[0]->getFailsafeStatus();
}

uint32_t LeafMultiPack::getVoltTempStatus() const
{
   return (m_vmonitor[0]->getVoltTempStatus());
}


float LeafMultiPack::getVoltage() const
{
   return m_vmonitor[0]->getVoltage();
}

float LeafMultiPack::getCurrent() const
{
   return m_vmonitor[0]->getCurrent();
}

float LeafMultiPack::getTemperature() const
{
   return  m_vmonitor[0]->getTemperature();
}

float LeafMultiPack::getSocPercent() const
{
   return m_vmonitor[0]->getSocPercent();
}

float LeafMultiPack::getSohPercent() const
{
   return m_vmonitor[0]->getSohPercent();
}

float LeafMultiPack::getEnergyRemainingKwh() const
{
   return m_vmonitor[0]->getEnergyRemainingKwh();
}

float LeafMultiPack::getCapacityKwh() const
{
   return m_vmonitor[0]->getCapacityKwh();
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
   return m_vmonitor[0]->getNominalCapacityKwh(); // NOMINAL_CAPACITY_KWH; // TODO - configurable
}

unsigned LeafMultiPack::getNumberOfModules() const
{
   return m_vmonitor[0]->getNumberOfModules(); // NUM_MODULES; // TODO - configurable
}

uint32_t LeafMultiPack::getManufacturingDateUnixTime() const
{
   return 2; // TODO - where do we get this from?
}

const char* LeafMultiPack::getManufacturerName() const
{
   return "TIML-LMP";
}

const char* LeafMultiPack::getBatteryName() const
{
   return "LeafG2-LMP";
}

float LeafMultiPack::getMaxChargeVoltage() const
{
   return m_vmonitor[0]->getMaxChargeVoltage(); 
}

float LeafMultiPack::getMinDischargeVoltage() const
{
   return m_vmonitor[0]->getMinDischargeVoltage(); 
}

float LeafMultiPack::getChargeCurrentLimit() const
{
   return m_vmonitor[0]->getChargeCurrentLimit();
}

float LeafMultiPack::getDischargeCurrentLimit() const
{
   return m_vmonitor[0]->getDischargeCurrentLimit();
}

contactor::Contactor& LeafMultiPack::getMainContactor()
{
   return m_main_contactor;
}

}
}



