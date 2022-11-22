/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <packs/Nissan/LeafMultiPack.hpp>

namespace packs {
namespace Nissan {

LeafMultiPack::LeafMultiPack(
            std::vector<monitor::Monitor*> vmonitor,
            std::vector<contactor::Contactor*> vsafetyshunt,
            core::Timer& timer,
            core::OutputPin& positive_relay,
            core::OutputPin& negative_relay,
            core::OutputPin& indicator,
            logging::ostream* log):

      m_vmonitor(vmonitor),
      m_vsafety_shunt(vsafetyshunt),
      m_timer(timer),
      m_main_contactor(
         timer,
         positive_relay,
         negative_relay,
         indicator,
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
      m_startup_callback_count(0)
{
   m_timer.registerPeriodicCallback(&m_periodic_callback, CALLBACK_PERIOD_ms);
   printf("\r\nStatus set to STARTUP.\n\r");
}

LeafMultiPack::~LeafMultiPack()
{
   m_timer.deregisterCallback(&m_periodic_callback);
}

void LeafMultiPack::periodicCallback()
{
   // data logging!
   // user interface data? (via sockets?)

   // start up proceedure... find healty batteries.

   // calculate inital values for big battery, to pass to inverter

   // close main contactor

   // run proceedure
   //    monitor batteries
   //    recalulate values for big battery
   //    operate shunt if neecessary (resend shunt trigger)
   //    open main contactor if necessary (extreme case, like loss fo USB comms)


   // printf("PERIODIC CALLBACK\n\r");
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
         // if (m_startup_callback_count<4) printf("\n\n");
         printf("\r\nStartup Sequence %2d  Packs Started %2u",
               MAX_STARTUP_COUNT-m_startup_callback_count,
               int(m_vmonitor.size()-pack_startup_fail));
         if ((pack_startup_fail == 0) ||(m_startup_callback_count > MAX_STARTUP_COUNT))
         {
            // DO WE WANT A REDUCED OPERATION STATUS if pack_startup_fail !=0 ?
            // m_multipack_status = Monitor::REDUCED_OPERATION'
            printf("\r\nStatus set to NORMAL_OPERATION.\n\r");
            m_multipack_status = Monitor::NORMAL_OPERATION;
            // check that there are normal packs (not all packs have failed to startup)
            if (pack_startup_fail < m_vmonitor.size())
            {
               // there are normal packs, close the main contactors
               m_main_contactor.setSafeToOperate(true);
            }
         }
         }
         break;

      case Monitor::NORMAL_OPERATION:
         // WHAT HAPPENS DURING NORMAL OPERATION?!?!?!?
         // the inverter is driving the operation...
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



   // if (m_multipack_status == Monitor::STARTUP)
   // {  
   //    m_startup_callback_count++;

   //    // see if any packs have failed to achieive normality
   //    uint pack_startup_fail = 0;
   //    for (uint i=0; i<m_vmonitor.size(); i++)
   //    {
   //       if (m_vmonitor[i]->getPackStatus() != Monitor::NORMAL_OPERATION)
   //       {
   //          pack_startup_fail++;
   //       }
   //    }
   //    if (pack_startup_fail == 0)
   //    {
   //       // all packs have achieved normality
   //       m_multipack_status = Monitor::NORMAL_OPERATION;
   //    }
   //    // if (m_startup_callback_count > MAX_STARTUP_COUNT && pack_startup_fail > 0)
   //    // {
   //    //    // DO WANT A REDUCED OPERATION STATUS? 
   //    //    m_multipack_status = Monitor::REDUCED_OPERATION'
   //    // }
   // }

   // else if (m_multipack_status == Monitor::NORMAL_OPERATION)
   // {
   // for (uint i=0; i<m_vmonitor.size(); i++)
   // {
   //    if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION)
   //    {

   //    }
   // }

   // }
}

monitor::Monitor::Pack_Status LeafMultiPack::getPackStatus() const
{
   return m_multipack_status;
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
   // calculate the average temperature for the packs
   float avg = 0;
   uint count = 0;
   for (uint i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == Monitor::NORMAL_OPERATION) {
         count++;
         avg += m_vmonitor[i]->getTemperature();
      }
   }
   if (count > 0) {
      return (avg / count);
   }
   else return NAN;
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

}
}



