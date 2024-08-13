/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef PACKS_NISSAN_LEAFMULTIPACK_HPP_
#define PACKS_NISSAN_LEAFMULTIPACK_HPP_

#include <math.h>
#include <vector>
#include <iostream>
#include "monitor/Monitor.hpp"
#include "contactor/Contactor.hpp"
#include "contactor/Nissan/LeafContactor.hpp"
#include "monitor/Nissan/LeafMonitor.hpp"

namespace packs {
namespace Nissan {

class LeafMultiPack: public monitor::Monitor
{
public:

   LeafMultiPack( std::vector<monitor::Monitor*> vmonitor,
                  std::vector<contactor::Contactor*> vcontactor,
                  core::Timer& timer,
                  core::OutputPin& positive_relay,
                  core::OutputPin& negative_relay,
                  core::OutputPin& pre_charge_relay,
                  logging::Logger *log);

   ~LeafMultiPack();

   // monitor::Monitor
   virtual Monitor::Pack_Status getPackStatus() const;
   virtual void setPackStatus(Pack_Status p);
   virtual void logStartupStatus() const;
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
   virtual float getMinCellVolts() const;
   virtual float getMaxCellVolts() const;

   virtual uint32_t getSystemVersion() const;
   virtual uint32_t getSerialNumber() const;
   virtual float getNominalCapacityKwh() const;
   virtual unsigned getNumberOfModules() const;
   virtual uint32_t getManufacturingDateUnixTime() const;
   virtual const char* getManufacturerName() const;
   virtual const char* getBatteryName() const;

   virtual uint32_t getVoltTempStatus() const;
   virtual uint32_t getFailsafeStatus() const;

   // no need for getMonitor as this (LeafMultiPack) is the montior
   // need to pass internal contactor to inverter
   contactor::Contactor& getMainContactor();

private:
   void periodicCallback();
   void updateFullyChargedDischargedStatus();

   std::vector<monitor::Monitor*>      m_vmonitor;
   std::vector<contactor::Contactor*>  m_vsafety_shunt;
   core::Timer&                        m_timer;
   contactor::Nissan::LeafContactor    m_main_contactor;

   logging ::Logger* m_log;

   core::Callback<LeafMultiPack> m_periodic_callback;
   // bool m_voltages_ok;
   // bool m_temperatures_ok;

   // float m_soc_percent;
   // float m_soh_percent;
   // float m_energy_remaining_kwh;
   // float m_capacity_kwh;

   // float m_current;
   // float m_voltage;
   // float m_average_temperature;

   float m_discharge_power_limit;
   float m_charge_power_limit;
   float m_discharge_current_limit;
   float m_charge_current_limit;

   bool m_fully_discharged;
   bool m_fully_charged;
   bool m_display_shutdown_status;

   Pack_Status m_multipack_status;

   uint m_startup_callback_count;
   const uint CALLBACK_PERIOD_ms = 1000;
   const uint MAX_STARTUP_COUNT = 10 * 60;  // number of callback periods 

};

}
}

#endif /* PACKS_NISSAN_LEAFMULTIPACK_HPP_ */
