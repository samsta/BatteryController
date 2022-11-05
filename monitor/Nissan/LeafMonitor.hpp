/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MONITOR_NISSAN_MONITOR_HPP
#define _MONITOR_NISSAN_MONITOR_HPP

#include "monitor/Monitor.hpp"
#include "can/FrameSink.hpp"
#include "can/messages/Nissan/Message.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/BatteryPowerLimits.hpp"

namespace contactor {
class Contactor;
}

namespace can {
namespace messages {
namespace Nissan {
class Message;
class CellVoltageRange;
class PackTemperatures;
class BatteryState;
class BatteryStatus;
class BatteryPowerLimits;
}
}
}

namespace monitor {
namespace Nissan {

class LeafMonitor: public monitor::Monitor, public can::messages::Nissan::MessageSink
{
public:
   explicit LeafMonitor(contactor::Contactor&);

   virtual void sink(const can::messages::Nissan::Message&);

   // monitor::Monitor
   virtual bool  isEverythingOk() const;
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
   
   uint32_t getVoltTempStatus() const;
   uint32_t getFailsafeStatus() const;

private:
   void process(const can::messages::Nissan::CellVoltageRange&);
   void process(const can::messages::Nissan::PackTemperatures&);
   void process(const can::messages::Nissan::BatteryState&);
   void process(const can::messages::Nissan::BatteryStatus&);
   void process(const can::messages::Nissan::BatteryPowerLimits&);
   void updateOperationalSafety();

   void calculateTemperatureLimitFactor(float min, float max);
   void calculateCurrentLimitByVoltage(float min, float max);

   contactor::Contactor& m_safety_shunt;
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

   float m_discharge_cur_fac_by_voltage;

   float m_discharge_power_limit;
   float m_charge_power_limit;
   float m_discharge_current_limit;
   float m_charge_current_limit;

   uint32_t m_volt_temp_status;
   uint32_t m_failsafe_status;
};

}
}

#endif // _MONITOR_NISSAN_MONITOR_HPP
