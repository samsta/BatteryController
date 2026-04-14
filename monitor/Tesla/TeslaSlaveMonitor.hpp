/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MONITOR_TESLA_SLAVE_MONITOR_HPP
#define _MONITOR_TESLA_SLAVE_MONITOR_HPP

#include "monitor/Monitor.hpp"
#include "can/FrameSink.hpp"
#include "can/messages/Tesla/TSTemperatures.hpp"
#include "can/messages/Tesla/TSCellVoltages.hpp"
#include "can/messages/Tesla/TSModCellVoltages.hpp"
#include "can/messages/Tesla/TSModVoltTemp.hpp"
#include "can/messages/Tesla/TSBatteryStatus.hpp"
#include "can/messages/Tesla/TSCurrentEnergy.hpp"
#include "can/messages/Tesla/Message.hpp"
// #include "can/messages/Nissan/BatteryState.hpp"
// #include "can/messages/Nissan/BatteryStatus.hpp"
// #include "can/messages/Nissan/CellVoltageRange.hpp"
// #include "can/messages/Nissan/PackTemperatures.hpp"
// #include "can/messages/Nissan/BatteryPowerLimits.hpp"
#include "logging/logging.hpp"

namespace contactor {
class Contactor;
}

namespace can {
namespace messages {
namespace Tesla {
class Message;
class TSTemperatures;
// class CellVoltageRange;
// class BatteryState;
// class BatteryStatus;
// class BatteryPowerLimits;
}
}
}

namespace monitor {
namespace Tesla {

class TeslaSlaveMonitor: public monitor::Monitor, public can::messages::Tesla::MessageSink
{
public:
   explicit TeslaSlaveMonitor(
            char *packname,
            // contactor::Contactor&,
            logging::Logger* log);

   virtual void sink(const can::messages::Tesla::Message&);

   void updateOperationalSafety();

   // monitor::Monitor
   virtual Monitor::Pack_Status getPackStatus() const;
   virtual void setPackStatus(Pack_Status packstat);
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
   std::string getAlarmConditionText() const;

private:
   void process(const can::messages::Tesla::TSTemperatures&);
   void process(const can::messages::Tesla::TSVoltages&);
   void process(const can::messages::Tesla::TSBatteryStatus&);
   void process(const can::messages::Tesla::TSCurrentEnergy&);
   void process(const can::messages::Tesla::TSModCellVoltages&);
   void process(const can::messages::Tesla::TSModVoltTemp&);

   char                    *m_pack_name;
   // contactor::Contactor&   m_safety_shunt;
   logging::Logger         *m_log;
   can::messages::Tesla::TSBatteryStatus::Battery_Status m_prev_battery_status;
   bool m_battery_status_ok;
   bool m_voltages_ok;
   bool m_temperatures_ok;
   bool m_bat_status_recv;
   Pack_Status m_pack_status;

   float m_soc_percent;
   float m_soh_percent;
   float m_energy_remaining_kwh;
   float m_capacity_kwh;

   float m_current;
   float m_voltage;
   float m_min_cell_volts;
   float m_max_cell_volts;
   float m_average_temperature;

   float m_discharge_cur_fac_by_voltage;

   float m_discharge_power_limit;
   float m_charge_power_limit;
   float m_discharge_current_limit;
   float m_charge_current_limit;
   const float MAX_ALLOWABLE_CURRENT = 25.0;

   uint32_t m_volt_temp_status;
   uint32_t m_failsafe_status;

   // can::messages::Tesla::TSModCellVoltages::CellVoltageData &m_mod_cell_data;
   // can::messages::Tesla::TSModVoltTemp::VoltTempData &m_mod_volt_temp_data;

   class CurrentLimitSmoothing {
   public:
      CurrentLimitSmoothing(float init_value);
      float process(float input);

   private:
      float m_max_current;
      uint16_t m_hist_index;
      static const uint16_t HIST_SIZE = 10;
      float m_hist_data[HIST_SIZE];

   };
   CurrentLimitSmoothing m_charge_cur_smoothing;
   CurrentLimitSmoothing m_discharge_cur_smoothing;
};

}
}

#endif // _MONITOR_TESLA_SLAVE_MONITOR_HPP
