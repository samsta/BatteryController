/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef PACKS_NISSAN_LEAFMULTIPACK_HPP_
#define PACKS_NISSAN_LEAFMULTIPACK_HPP_

#include <math.h>
#include "monitor/Monitor.hpp"
#include "contactor/Contactor.hpp"
#include "contactor/Nissan/LeafContactor.hpp"
#include "monitor/Nissan/LeafMonitor.hpp"

namespace packs {
namespace Nissan {

class LeafMultiPack: public monitor::Monitor, public contactor::Contactor
{
public:
   // LeafMultiPack(
   //                 monitor::Monitor& monitor1,
   //                 contactor::Contactor& contactor1,
   //                 monitor::Monitor& monitor2,
   //                 contactor::Contactor& contactor2,
   //                 logging::ostream* log = nullptr);

   LeafMultiPack( unsigned int num_packs,
                  monitor::Monitor& monitor1,
                  contactor::Contactor& contactor1,
                  monitor::Monitor& monitor2,
                  contactor::Contactor& contactor2,
                  monitor::Monitor& monitor3,
                  contactor::Contactor& contactor3,
                  logging::ostream* log = nullptr);

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

   virtual uint32_t getContactorStatus() const;

   // no need for getMonitor as this (LeafMultiPack) is the montior
   // need to pass internal contactor to inverter
   contactor::Contactor& getContactor();

   // static const unsigned NUM_PACKS = 3;

private:
   unsigned int m_num_packs;

   // input from battery packs
   monitor::Monitor&     m_1monitor;
   contactor::Contactor& m_1contactor;
   monitor::Monitor&     m_2monitor;
   contactor::Contactor& m_2contactor;
   monitor::Monitor&     m_3monitor;
   contactor::Contactor& m_3contactor;
   logging::ostream*     m_log;

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

   private:
      enum State {
      OPEN,
      CLOSING,
      CLOSED
   };

   virtual void setSafeToOperate(bool);
   virtual bool isSafeToOperate() const;
   virtual bool isClosed() const;
   virtual void close();
   virtual void open();
   void updateRelays();

   bool  m_safe_to_operate;
   State m_requested_state;
   State m_state;

};

}
}

#endif /* PACKS_NISSAN_LEAFMULTIPACK_HPP_ */
