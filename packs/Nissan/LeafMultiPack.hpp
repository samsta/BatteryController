/*
 * LeafMultiPack.hpp
 *
 *  Created on: Sep 5, 2022
 *      Author: openonevm
 */

#ifndef PACKS_NISSAN_LEAFMULTIPACK_HPP_
#define PACKS_NISSAN_LEAFMULTIPACK_HPP_

#include "packs/Nissan/LeafPack.hpp"

namespace packs {
namespace Nissan {

class LeafMultiPack : public monitor::Monitor
{
public:
   LeafMultiPack();
   ~LeafMultiPack();

   // monitor::Monitor
   virtual float getVoltage() const;
   virtual float getCurrent() const;
   virtual float getTemperature() const;
   virtual float getSocPercent() const;
   virtual float getSohPercent() const;
   virtual float getEnergyRemainingKwh() const;
   virtual float getCapacityKwh() const;
   virtual float getMaxChargeVoltage() const;
   virtual float getMinDischargeVoltage() const;
   virtual float getChargeCurrentLimit() const;
   virtual float getDischargeCurrentLimit() const;

   virtual uint32_t getSystemVersion() const;
   virtual uint32_t getSerialNumber() const;
   virtual float getNominalCapacityKwh() const;
   virtual unsigned getNumberOfModules() const;
   virtual uint32_t getManufacturingDateUnixTime() const;
   virtual const char* getManufacturerName() const;
   virtual const char* getBatteryName() const;

//   monitor::Monitor& getMonitor(int index);
//   contactor::Contactor& getContactor();

   static const unsigned NUM_PACKS = 3;

private:

   bool m_voltages_ok;
   bool m_temperatures_ok;
   bool m_everything_ok;

   float m_soc_percent;
   float m_soh_percent;
   float m_energy_remaining_kwh;
   float m_capacity_kwh;

   float m_current;
   float m_voltage;
   float m_average_temperature;

   float m_cur_fac_by_temperature;
   float m_charge_cur_fac_by_voltage;
   float m_discharge_cur_fac_by_voltage;

   float m_discharge_power_limit;
   float m_charge_power_limit;
   float m_discharge_current_limit;
   float m_charge_current_limit;



   packs::Nissan::LeafPack                m_packs[NUM_PACKS];
   contactor::Nissan::LeafContactor       m_contactor;
   monitor::Nissan::LeafMonitor               m_monitor;

};

}
}





#endif /* PACKS_NISSAN_LEAFMULTIPACK_HPP_ */
