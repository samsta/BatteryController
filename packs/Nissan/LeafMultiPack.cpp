/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafMultiPack.hpp>

namespace packs {
namespace Nissan {

LeafMultiPack::LeafMultiPack(
            std::vector<monitor::Monitor*> vmonitor,
            core::Timer& timer,
            core::OutputPin& positive_relay,
            core::OutputPin& negative_relay,
            core::OutputPin& pre_charge_relay,
            logging::Logger *log):

      m_vmonitor(vmonitor),
      m_timer(timer),
      m_main_contactor(
         timer,
         positive_relay,
         negative_relay,
         pre_charge_relay,
         log),
      m_log(log),
      m_periodic_callback(*this, &LeafMultiPack::periodicCallback),
      // m_voltages_ok(false),
      // m_temperatures_ok(false),
      // m_soc_percent(NAN),
      // m_soh_percent(NAN),
      // m_energy_remaining_kwh(NAN),
      // m_capacity_kwh(NAN),
      // m_current(NAN),
      // m_voltage(NAN),
      // m_average_temperature(NAN),
      m_discharge_power_limit(NAN),
      m_charge_power_limit(NAN),
      m_discharge_current_limit(0),
      m_charge_current_limit(0),
      m_multipack_status(monitor::Monitor::STARTUP),
      m_startup_callback_count(0),
      m_fully_charged(true),
      m_fully_discharged(true)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, CALLBACK_PERIOD_ms, "LeafMultiPackPeriodic");
   if (m_log) m_log->info("LeafMultiPack: status set to STARTUP");
   std::ostringstream ss,sss;
   ss << "LeafMultiPack: pack startup wait time = " << CALLBACK_PERIOD_ms * MAX_STARTUP_COUNT / 1000 << " seconds";
   if (m_log) m_log->info(ss);
   sss << "LeafMultiPack: number of packs: " << (int(m_vmonitor.size()));
   if (m_log) m_log->info(sss);
}

LeafMultiPack::~LeafMultiPack()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void LeafMultiPack::periodicCallback()
{
   // user interface data? (via sockets?)

   // start up proceedure... find healty batteries.

   // calculate inital values for big battery, to pass to inverter

   // close main contactor

   // run proceedure
   //    monitor batteries
   //    recalulate values for big battery
   //    operate shunt if neecessary (resend shunt trigger)
   //    open main contactor if necessary (extreme case, like loss fo USB comms)

   switch (m_multipack_status) {

      case Monitor::STARTUP:
         {
         uint pack_startup_fail = 0;
         m_startup_callback_count++;

         // see if any packs have failed to achieive normality
         for (uint i=0; i<m_vmonitor.size(); i++)
         {
            if (m_vmonitor[i]->getPackStatus() != Monitor::NORMAL_OPERATION)
            {
               pack_startup_fail++;
            }
         }
         std::ostringstream ss;
         ss << "LeafMultiPack: Startup Sequence:" << m_startup_callback_count << "  Packs Started:"
               << (int(m_vmonitor.size()-pack_startup_fail));
         if (m_log) m_log->info(ss);
         if ((pack_startup_fail == 0) ||(m_startup_callback_count > MAX_STARTUP_COUNT))
         {
            // DO WE WANT A REDUCED OPERATION STATUS if pack_startup_fail !=0 ?
            // check that there are normal packs (that is, not all packs have failed to startup)
            if (pack_startup_fail < m_vmonitor.size())
            {
               // there are normal packs, give the 'ok' to close the main contactors (doesn't close the contractors)
               m_multipack_status = Monitor::NORMAL_OPERATION;
               m_main_contactor.setSafeToOperate(true);
               if (m_log) m_log->info("LeafMultiPack: status set to NORMAL_OPERATION");
            }
            else
            {
               // no packs started, running is pointless
               m_multipack_status = Monitor::STARTUP_FAILED;
               m_main_contactor.setSafeToOperate(false);
               if (m_log) m_log->info("LeafMultiPack: status set to STARTUP_FAILED");

            }
         }
         }
         break;

      case Monitor::NORMAL_OPERATION:
         // WHAT HAPPENS DURING NORMAL OPERATION?!?!?!?
         // the inverter is driving the operation by polling for data...

         updateFullyChargedDischargedStatus();

         for (uint i=0; i<m_vmonitor.size(); i++)
         {
            if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION)
            {

            }
         }
         break;

      case Monitor::SHUNT_ACTIVIATED:
      case Monitor::SHUNT_ACT_FAILED:
      case Monitor::SHUTDOWN:
      default:
         break;
   }
}

void LeafMultiPack::updateFullyChargedDischargedStatus()
{
   // check/set fully charged/discharged status with hysteresis
   if (getSocPercent() > 25.0) {
      m_fully_discharged = false;
   }
   else if (getSocPercent() < 20.0) {
      m_fully_discharged = true;
   }
   if (getSocPercent() < 75.0) {
      m_fully_charged = false;
   }
   else if (getSocPercent() > 80.0) {
      m_fully_charged = true;
   }
}

monitor::Monitor::Pack_Status LeafMultiPack::getPackStatus() const
{
   return m_multipack_status;
}

void LeafMultiPack::setPackStatus(Monitor::Pack_Status p)
{
   m_multipack_status = p;
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
   // calculate the average voltage for the packs
   float avg = 0;
   uint count = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         count++;
         avg += m_vmonitor[i]->getVoltage();
      }
   }
   if (count > 0) {
      return (avg / count);
   }
   else return NAN;
}

float LeafMultiPack::getCurrent() const
{
   // calculate the total for the packs
   float total = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         total += m_vmonitor[i]->getCurrent();
      }
   }
   return total;
}

float LeafMultiPack::getMinCellVolts() const
{
   // calculate the min for the packs
   float min = 99.0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         if (m_vmonitor[i]->getMinCellVolts() < min) min = (m_vmonitor[i]->getMinCellVolts());
      }
   }
   return min;
}

float LeafMultiPack::getMaxCellVolts() const
{
   // calculate the max for the packs
   float max = 0.0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         if (m_vmonitor[i]->getMaxCellVolts() > max) max = (m_vmonitor[i]->getMaxCellVolts());
      }
   }
   return max;
}

float LeafMultiPack::getMaxChargeVoltage() const
{
   // this is a constant
   return m_vmonitor[0]->getMaxChargeVoltage(); 
}

float LeafMultiPack::getMinDischargeVoltage() const
{
   // this is a constant
   return m_vmonitor[0]->getMinDischargeVoltage(); 
}

float LeafMultiPack::getChargeCurrentLimit() const
{
   // if full, no charging allowed
   if (m_fully_charged) return 0.0;

   // calculate the total for the packs
   float total = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         total += m_vmonitor[i]->getChargeCurrentLimit();
      }
   }
   return total;
}

float LeafMultiPack::getDischargeCurrentLimit() const
{
   // if empty, no discharging allowed
   if (m_fully_discharged) return 0.0;

   // calculate the total for the packs
   float total = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         total += m_vmonitor[i]->getDischargeCurrentLimit();
      }
   }
   return total;
}

float LeafMultiPack::getTemperature() const
{
   // calculate the max temperature for the packs
   float max = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         if (m_vmonitor[i]->getTemperature() > max) max = (m_vmonitor[i]->getTemperature());
      }
   }
   return max;
}

float LeafMultiPack::getSocPercent() const
{
   // calculate the average for the packs
   float avg = 0;
   uint count = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         count++;
         avg += m_vmonitor[i]->getSocPercent();
      }
   }
   if (count > 0) {
      return (avg / count);
   }
   else return NAN;
}

float LeafMultiPack::getSohPercent() const
{
   // calculate the average for the packs
   float avg = 0;
   uint count = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         count++;
         avg += m_vmonitor[i]->getSohPercent();
      }
   }
   if (count > 0) {
      return (avg / count);
   }
   else return NAN;
}

float LeafMultiPack::getEnergyRemainingKwh() const
{
   // calculate the total for the packs
   float total = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         total += m_vmonitor[i]->getEnergyRemainingKwh();
      }
   }
   return total;
}

float LeafMultiPack::getCapacityKwh() const
{
   // calculate the total for the packs
   float total = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         total += m_vmonitor[i]->getCapacityKwh();
      }
   }
   return total;
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

contactor::Contactor& LeafMultiPack::getMainContactor()
{
   return m_main_contactor;
}

void LeafMultiPack::logStartupStatus() const
{
}

}
}



