/*
ADD TESTS FOR WARN HIGH/LOW VOLTAGE AND WARN HIGH/LOW TEMPS
ADD LOGGING MESSAGES FOR ALL WARN AND CRITICAL VOLTS/TEMPS
ADD LOGGING OF VOLTS TEMPS AT SOME REASONABLE INTERVAL (5 MIN?  15 MIN?)
*/




/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "contactor/Contactor.hpp"
#include <math.h>
#include <monitor/Nissan/LeafMonitor.hpp>

using namespace can::messages::Nissan;

namespace monitor {
namespace Nissan {

namespace {

const float CRITICALLY_HIGH_VOLTAGE(4.15);
const float WARN_HIGH_VOLTAGE(4.1);
const float WARN_LOW_VOLTAGE(3.3);
const float CRITICALLY_LOW_VOLTAGE(3);
const float CRITICALLY_HIGH_VOLTAGE_SPREAD(0.5);

const uint32_t CRIT_HIGH_VOLT(1 << 5);
const uint32_t CRIT_LOW_VOLT(1 << 4);
const uint32_t CRIT_SPREAD_VOLT(1 << 3);
const uint32_t CRIT_HIGH_TEMP(1 << 2);
const uint32_t CRIT_LOW_TEMP(1 << 1);
const uint32_t MAX_TEMP_MISSING(1 << 0);

const float CRITICALLY_HIGH_TEMPERATURE(50);
const float WARN_HIGH_TEMPERATURE(40);
const float WARN_LOW_TEMPERATURE(5);
const float CRITICALLY_LOW_TEMPERATURE(2);
const float MAX_TEMP_SENSORS_MISSING(1);

const float NOMINAL_CAPACITY_KWH(24);
//const float NOMINAL_CURRENT_LIMIT(20);

const unsigned NUM_MODULES(24);
const unsigned NUM_CELLS(96);

const float TEMPERATURE_LIMIT_RESOLUTION(1);
const float VOLTAGE_LIMIT_RESOLUTION(0.001);

inline float upper_limit(float value, const float warn_limit, const float critical_limit, const float resolution)
{
   return (critical_limit - value) / (critical_limit - warn_limit + resolution);
}

inline float lower_limit(float value, const float warn_limit, const float critical_limit, const float resolution)
{
   return (value - critical_limit) / (warn_limit - critical_limit + resolution);
}
}

LeafMonitor::LeafMonitor(
      char *packname,
      contactor::Contactor& safety_shunt,
      logging::Logger* log):
      m_pack_name(packname),
      m_safety_shunt(safety_shunt),
      m_log(log),
      m_voltages_ok(false),
      m_temperatures_ok(false),
      m_pack_status(STARTUP),
      m_soc_percent(NAN),
      m_soh_percent(NAN),
      m_energy_remaining_kwh(NAN),
      m_capacity_kwh(NAN),
      m_current(NAN),
      m_voltage(NAN),
      m_average_temperature(NAN),
      m_discharge_cur_fac_by_voltage(0),
      m_discharge_power_limit(NAN),
      m_charge_power_limit(NAN),
      m_discharge_current_limit(0),
      m_charge_current_limit(0),
	   m_volt_temp_status(pow(2,6)-1),
      m_failsafe_status(7)
{
}

void LeafMonitor::sink(const can::messages::Nissan::Message& message)
{
   if (not message.valid()) return;

   switch(message.id())
   {
   case ID_BATTERY_STATUS:
      process(static_cast<const BatteryStatus&>(message));
      break;
   case ID_LBC_POWER_LIMITS:
      process(static_cast<const BatteryPowerLimits&>(message));
      break;
   case ID_LBC_DATA_REPLY:
      // handled in next switch
      break;
   default:
      // unknown id
      return;
   }

   switch(message.dataGroup())
   {
   case GROUP_CELL_VOLTAGE_RANGE:
      process(static_cast<const CellVoltageRange&>(message));
      return;
   case GROUP_PACK_TEMPERATURES:
      process(static_cast<const PackTemperatures&>(message));
      return;
   case GROUP_BATTERY_STATE:
      process(static_cast<const BatteryState&>(message));
      return;
   default:
      break;
   }
}

void LeafMonitor::process(const CellVoltageRange& voltage_range)
{
   if (voltage_range.getMax() < CRITICALLY_HIGH_VOLTAGE &&
       voltage_range.getMin() > CRITICALLY_LOW_VOLTAGE    &&
       (voltage_range.getMax() - voltage_range.getMin()) < CRITICALLY_HIGH_VOLTAGE_SPREAD)
   {
      m_voltages_ok = true;
   }
   else
   {
      m_voltages_ok = false;
   }

   if (voltage_range.getMax() < CRITICALLY_HIGH_VOLTAGE) m_volt_temp_status &= ~CRIT_HIGH_VOLT;
   else m_volt_temp_status |= CRIT_HIGH_VOLT;

   if (voltage_range.getMin() > CRITICALLY_LOW_VOLTAGE ) m_volt_temp_status &= ~CRIT_LOW_VOLT;
   else m_volt_temp_status |= CRIT_LOW_VOLT;

   if ((voltage_range.getMax() - voltage_range.getMin()) < CRITICALLY_HIGH_VOLTAGE_SPREAD ) m_volt_temp_status &= ~CRIT_SPREAD_VOLT;
   else m_volt_temp_status |= CRIT_SPREAD_VOLT;

   updateOperationalSafety();
}

void LeafMonitor::process(const PackTemperatures& temperatures)
{
   unsigned num_sensors_missing = 0;
   float max_temp = NAN, min_temp, accumulated_temp = NAN;
   for (unsigned k = 0; k < temperatures.NUM_SENSORS; k++)
   {
      float temp = temperatures.getTemperature(k);

      if (isnan(max_temp))
      {
         min_temp = max_temp = temp;
      }
      else
      {
         if (temp > max_temp) max_temp = temp;
         if (temp < min_temp) min_temp = temp;
      }

      if (isnan(temp))
      {
         num_sensors_missing++;
      }
      else if (isnan(accumulated_temp))
      {
         accumulated_temp = temp;
      }
      else
      {
         accumulated_temp += temp;
      }
   }

   m_average_temperature = accumulated_temp / (temperatures.NUM_SENSORS - num_sensors_missing);

   if (max_temp < CRITICALLY_HIGH_TEMPERATURE &&
       min_temp > CRITICALLY_LOW_TEMPERATURE  &&
       num_sensors_missing <= MAX_TEMP_SENSORS_MISSING)
   {
      m_temperatures_ok = true;
   }
   else
   {
      m_temperatures_ok = false;
   }

   if (max_temp < CRITICALLY_HIGH_TEMPERATURE) m_volt_temp_status &= ~CRIT_HIGH_TEMP;
   else m_volt_temp_status |= CRIT_HIGH_TEMP;

   if (min_temp > CRITICALLY_LOW_TEMPERATURE) m_volt_temp_status &= ~CRIT_LOW_TEMP;
   else m_volt_temp_status |= CRIT_LOW_TEMP;

   if (num_sensors_missing <= MAX_TEMP_SENSORS_MISSING) m_volt_temp_status &= ~MAX_TEMP_MISSING;
   else m_volt_temp_status |= MAX_TEMP_MISSING;

   updateOperationalSafety();
}

void LeafMonitor::process(const BatteryState& battery_state)
{
   //m_soc_percent = battery_state.getSocPercent(); replaced with useable_soc
   m_soh_percent = battery_state.getHealthPercent();
   m_capacity_kwh = (NOMINAL_CAPACITY_KWH/100) * m_soh_percent;
   //m_energy_remaining_kwh = (m_capacity_kwh/100) * m_soc_percent;
}

void LeafMonitor::process(const BatteryStatus& battery_status)
{
   m_current = battery_status.getCurrent();
   m_voltage = battery_status.getVoltage();
   m_soc_percent = (float)battery_status.getUsableSOC();
   m_energy_remaining_kwh = (m_capacity_kwh/100) * m_soc_percent;
   m_failsafe_status = battery_status.getFailsafeStatus();
}

void LeafMonitor::process(const BatteryPowerLimits& battery_power)
{
   m_discharge_power_limit = battery_power.getDischargePowerLimit_kW();
   m_charge_power_limit = battery_power.getChargePowerLimit_kW();

   if (m_voltage > 0)
   {
      m_discharge_current_limit = m_discharge_power_limit * 1000.0 / m_voltage;
      m_charge_current_limit = m_charge_power_limit * 1000.0 / m_voltage;
   }
}

void LeafMonitor::updateOperationalSafety()
{
   if (!m_safety_shunt.isSafeToOperate() && m_pack_status == Monitor::STARTUP)
   {
      m_pack_status = Monitor::SHUNT_ACTIVIATED;
      std::string ss;
      ss.append("LeafMonitor: ");
      ss.append(m_pack_name);
      ss.append(": SHUNT ACTIVIATED during STARTUP");
      if (m_log) m_log->alarm(ss.c_str(), __FILENAME__,__LINE__);
      // in multipack shunt safe to operate should be monitored
      // if shunt activated and current !=0, need to alarm or something
   }

   bool everything_ok = m_voltages_ok && m_temperatures_ok;
   if (!everything_ok && m_pack_status == Monitor::NORMAL_OPERATION )
   {
      // everything WAS ok, but now it isn't, trigger the safety shunt
      m_safety_shunt.setSafeToOperate(false);
      m_pack_status = Monitor::SHUNT_ACTIVIATED;
      // in multipack shunt safe to operate should be monitored
      // if shunt activated and current !=0, need to alarm or something
      std::string ss;
      ss.append("LeafMonitor: ");
      ss.append(m_pack_name);
      ss.append(": SHUNT ACTIVIATED during NORMAL operation");
      if (m_log) m_log->alarm(ss.c_str(), __FILENAME__,__LINE__);
   }
   else if (everything_ok && m_pack_status == Monitor::STARTUP)
   {
      // battery has come right on startup
      m_pack_status = Monitor::NORMAL_OPERATION;
      std::string ss;
      ss.append("LeafMonitor: ");
      ss.append(m_pack_name);
      ss.append(": status set to NORMAL, STARTUP completed");
      if (m_log) m_log->info(ss.c_str(), __FILENAME__,__LINE__);
   }
}

// void LeafMonitor::setPackStatus(Monitor::Pack_Status p)
// {
//    m_pack_status = p;
// }

Monitor::Pack_Status LeafMonitor::getPackStatus() const
{
   return m_pack_status;
}

float LeafMonitor::getVoltage() const
{
   return m_voltage;
}

float LeafMonitor::getCurrent() const
{
   return m_current;
}

float LeafMonitor::getTemperature() const
{
   return m_average_temperature;
}

float LeafMonitor::getSocPercent() const
{
   return m_soc_percent;
}

float LeafMonitor::getSohPercent() const
{
   return m_soh_percent;
}

float LeafMonitor::getEnergyRemainingKwh() const
{
   return m_energy_remaining_kwh;
}

float LeafMonitor::getCapacityKwh() const
{
   return m_capacity_kwh;
}

uint32_t LeafMonitor::getSystemVersion() const
{
   return 1; // TODO - come up with a versioning scheme
}

uint32_t LeafMonitor::getSerialNumber() const
{
   return 1; // TODO - do we configure this, or can we get it from the battery?
}

float LeafMonitor::getNominalCapacityKwh() const
{
   return NOMINAL_CAPACITY_KWH; // TODO - configurable
}

unsigned LeafMonitor::getNumberOfModules() const
{
   return NUM_MODULES; // TODO - configurable
}

uint32_t LeafMonitor::getManufacturingDateUnixTime() const
{
   return 1; // TODO - where do we get this from?
}

const char* LeafMonitor::getManufacturerName() const
{
   return "TIML-LM";
}

const char* LeafMonitor::getBatteryName() const
{
   return "LeafG2-LM";
}

float LeafMonitor::getMaxChargeVoltage() const
{
   return CRITICALLY_HIGH_VOLTAGE * NUM_CELLS;
}

float LeafMonitor::getMinDischargeVoltage() const
{
   return CRITICALLY_LOW_VOLTAGE * NUM_CELLS;
}

float LeafMonitor::getChargeCurrentLimit() const
{
   return m_charge_current_limit;
}

float LeafMonitor::getDischargeCurrentLimit() const
{
   return m_discharge_current_limit;
}

uint32_t LeafMonitor::getVoltTempStatus() const
{
	return m_volt_temp_status;
}

uint32_t LeafMonitor::getFailsafeStatus() const
{
	return m_failsafe_status;
}
}
}
