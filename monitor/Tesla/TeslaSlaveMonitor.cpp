/*
ADD TESTS FOR WARN HIGH/LOW VOLTAGE AND WARN HIGH/LOW TEMPS
ADD LOGGING MESSAGES FOR ALL WARN AND CRITICAL VOLTS/TEMPS
*/

/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "contactor/Contactor.hpp"
#include "logging/Hex.hpp"
#include <math.h>
#include <monitor/Tesla/TeslaSlaveMonitor.hpp>
#include <numeric>

using namespace can::messages::Tesla;

namespace monitor {
namespace Tesla {

namespace {

const float CRITICALLY_HIGH_VOLTAGE(4.15);
const float WARN_HIGH_VOLTAGE(4.1);
const float WARN_LOW_VOLTAGE(3.3);
const float CRITICALLY_LOW_VOLTAGE(3);
const float CRITICALLY_HIGH_VOLTAGE_SPREAD(0.5);

// assign bits for each status
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

TeslaSlaveMonitor::TeslaSlaveMonitor(
      char *packname,
      contactor::Contactor& safety_shunt,
      logging::Logger* log):
      m_pack_name(packname),
      m_safety_shunt(safety_shunt),
      m_log(log),
      m_battery_status_ok(false),
      m_voltages_ok(false),
      m_temperatures_ok(false),
      m_pack_status(STARTUP),
      m_soc_percent(NAN),
      m_soh_percent(NAN),
      m_energy_remaining_kwh(NAN),
      m_capacity_kwh(NAN),
      m_current(NAN),
      m_voltage(NAN),
      m_min_cell_volts(NAN),
      m_max_cell_volts(NAN),
      m_average_temperature(NAN),
      m_discharge_cur_fac_by_voltage(0),
      m_discharge_power_limit(NAN),
      m_charge_power_limit(NAN),
      m_discharge_current_limit(0),
      m_charge_current_limit(0),
	   m_volt_temp_status(pow(2,6)-1),
      m_failsafe_status(7),
      m_bat_status_recv(false),
      m_bat_temps_recv(false),
      m_bat_volts_recv(false),
      m_charge_cur_smoothing(MAX_ALLOWABLE_CURRENT),
      m_discharge_cur_smoothing(MAX_ALLOWABLE_CURRENT)
{
   m_volt_temp_status &= ~MAX_TEMP_MISSING; // clear max temp missing at startup
}

void TeslaSlaveMonitor::sink(const can::messages::Tesla::Message& message)
{
   if (not message.valid()) return;

   switch(message.id())
   {
   case ID_TS_TEMPS:
      process(static_cast<const TSTemperatures&>(message));
      break;

   case ID_TS_CELL_VOLT:
      process(static_cast<const TSVoltages&>(message));
      break;

   case ID_TS_BATTERY_STATUS:
      process(static_cast<const TSBatteryStatus&>(message));
      break;

   // case ID_LBC_POWER_LIMITS:
   //    process(static_cast<const BatteryPowerLimits&>(message));
   //    break;
   default:
      // unknown id
      return;
   }
}

void TeslaSlaveMonitor::process(const TSBatteryStatus& battery_status)
{
   m_bat_status_recv = true;
   uint8_t status = battery_status.getBatteryStatus();

   std::ostringstream ss;
   ss << "TeslaSlaveMonitor: " << m_pack_name << ": Processing Battery Status: Status=" << (int)status;
   if (m_log) m_log->debug(ss);

   if (status == 1) // OK
   {
      m_battery_status_ok = true;
   }
   else
   {
      m_battery_status_ok = false;
      // some fault condition
      std::ostringstream sss;
      sss << "TeslaSlaveMonitor: " << m_pack_name << ": Battery Status indicates FAULT condition: Status=" << (int)status;
      if (m_log) m_log->alarm(sss, __FILENAME__,__LINE__);
   }
   updateOperationalSafety();
}

void TeslaSlaveMonitor::process(const TSVoltages& voltages)
{
   m_bat_volts_recv = true;
   m_min_cell_volts = voltages.getMinCellVoltage();
   m_max_cell_volts = voltages.getMaxCellVoltage();
   m_voltage        = voltages.getPackVoltage();

   if (m_max_cell_volts < CRITICALLY_HIGH_VOLTAGE &&
       m_min_cell_volts > CRITICALLY_LOW_VOLTAGE    &&
       (m_max_cell_volts - m_min_cell_volts) < CRITICALLY_HIGH_VOLTAGE_SPREAD)
   {
      m_voltages_ok = true;
   }
   else
   {
      m_voltages_ok = false;
   }

   std::ostringstream ss;
   ss << "TeslaSlaveMonitor: " << m_pack_name << ": Processing Cell Voltages: Max=" << m_max_cell_volts << "V, Min=" << m_min_cell_volts << "V";
   if (m_log) m_log->debug(ss);
   
   if (m_max_cell_volts < CRITICALLY_HIGH_VOLTAGE) m_volt_temp_status &= ~CRIT_HIGH_VOLT;
   else m_volt_temp_status |= CRIT_HIGH_VOLT;

   if (m_min_cell_volts > CRITICALLY_LOW_VOLTAGE ) m_volt_temp_status &= ~CRIT_LOW_VOLT;
   else m_volt_temp_status |= CRIT_LOW_VOLT;

   if ((m_max_cell_volts - m_min_cell_volts) < CRITICALLY_HIGH_VOLTAGE_SPREAD ) m_volt_temp_status &= ~CRIT_SPREAD_VOLT;
   else m_volt_temp_status |= CRIT_SPREAD_VOLT;

   updateOperationalSafety();
}

void TeslaSlaveMonitor::process(const TSTemperatures& temperatures)
{
   m_bat_temps_recv = true;
   float max_temp = temperatures.getMaxTemperature();
   float min_temp = temperatures.getMinTempeature();
   m_average_temperature = temperatures.getAvgTemperature();
   
   std::ostringstream ss;
   ss << "TeslaSlaveMonitor: " << m_pack_name << ": Processing Temperatures: Max=" << max_temp << " degC, Min=" << min_temp << " degC";
   if (m_log) m_log->debug(ss);

   if (max_temp < CRITICALLY_HIGH_TEMPERATURE && min_temp > CRITICALLY_LOW_TEMPERATURE )
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

   updateOperationalSafety();
}

// void TeslaSlaveMonitor::process(const BatteryState& battery_state)
// {
//    m_bat_state_recv = true;
//    //m_soc_percent = battery_state.getSocPercent(); replaced with useable_soc
//    m_soh_percent = battery_state.getHealthPercent();
//    m_capacity_kwh = (NOMINAL_CAPACITY_KWH/100) * m_soh_percent;
//    //m_energy_remaining_kwh = (m_capacity_kwh/100) * m_soc_percent;
// }

// void TeslaSlaveMonitor::process(const BatteryStatus& battery_status)
// {
//    m_bat_status_recv = true;
//    m_current = battery_status.getCurrent();
//    m_voltage = battery_status.getVoltage();
//    m_soc_percent = (float)battery_status.getUsableSOC();
//    m_energy_remaining_kwh = (m_capacity_kwh/100) * m_soc_percent;
//    m_failsafe_status = battery_status.getFailsafeStatus();
// }

// void TeslaSlaveMonitor::process(const BatteryPowerLimits& battery_power)
// {
//    m_bat_limits_recv = true;
//    m_discharge_power_limit = battery_power.getDischargePowerLimit_kW();
//    m_charge_power_limit = battery_power.getChargePowerLimit_kW();

//    if (m_voltage > 0)
//    {
//       m_discharge_current_limit = m_discharge_power_limit * 1000.0 / m_voltage;
//       m_charge_current_limit = m_charge_power_limit * 1000.0 / m_voltage;
//    }
//    m_discharge_current_limit = m_discharge_cur_smoothing.process(m_discharge_current_limit);
//    m_charge_current_limit = m_charge_cur_smoothing.process(m_charge_current_limit);

//    // impose max value on the current limits
//    if (m_discharge_current_limit > MAX_ALLOWABLE_CURRENT) m_discharge_current_limit = MAX_ALLOWABLE_CURRENT;
//    if (m_charge_current_limit > MAX_ALLOWABLE_CURRENT) m_charge_current_limit = MAX_ALLOWABLE_CURRENT;
// }

void TeslaSlaveMonitor::updateOperationalSafety()
{
   if (!m_safety_shunt.isSafeToOperate() && m_pack_status == Monitor::STARTUP)
   {
      setPackStatus(Monitor::SHUNT_ACTIVIATED);
      std::string ss;
      ss.append("TeslaSlaveMonitor: ");
      ss.append(m_pack_name);
      ss.append(": SHUNT ACTIVIATED during STARTUP");
      if (m_log) m_log->alarm(ss, __FILENAME__,__LINE__);
   }

   if (!m_safety_shunt.isSafeToOperate() && m_pack_status == Monitor::NORMAL_OPERATION )
   {
      // safety shunt has already have been triggered when state was changed to false
      // just change pack status and report
      setPackStatus(Monitor::SHUNT_ACTIVIATED);
      std::string s2;
      s2.append("TeslaSlaveMonitor: ");
      s2.append(m_pack_name);
      s2.append(": SHUNT ACTIVIATED during NORMAL operation");
      if (m_log) m_log->alarm(s2, __FILENAME__,__LINE__);
   }

   bool everything_ok = m_battery_status_ok && m_voltages_ok && m_temperatures_ok && m_safety_shunt.isSafeToOperate();
   if (!everything_ok && m_pack_status == Monitor::NORMAL_OPERATION )
   {
      // everything WAS ok, but now it isn't, trigger the safety shunt
      m_safety_shunt.setSafeToOperate(false);
      setPackStatus(Monitor::SHUNT_ACTIVIATED);
      std::string s2;
      s2.append("TeslaSlaveMonitor: ");
      s2.append(m_pack_name);
      s2.append(": SHUNT ACTIVIATED during NORMAL operation");
      if (m_log) m_log->alarm(s2, __FILENAME__,__LINE__);

      // report an alarm about the issue
      std::string s1;
      s1.append("TeslaSlaveMonitor: ");
      s1.append(m_pack_name);
      s1.append(":  Alarm Condition(s) Present:");
      s1.append(getAlarmConditionText());
      if (m_log) m_log->alarm(s1, __FILENAME__,__LINE__);
   }
   else if (m_bat_status_recv && m_bat_temps_recv && m_bat_volts_recv && everything_ok && m_pack_status == Monitor::STARTUP)
   {
      // battery has come right on startup
      setPackStatus(Monitor::NORMAL_OPERATION);
      std::string ss;
      ss.append("TeslaSlaveMonitor: ");
      ss.append(m_pack_name);
      ss.append(": status set to NORMAL, STARTUP completed");
      if (m_log) m_log->info(ss.c_str(), __FILENAME__,__LINE__);
   }
}

Monitor::Pack_Status TeslaSlaveMonitor::getPackStatus() const
{
   return m_pack_status;
}

void TeslaSlaveMonitor::setPackStatus(Monitor::Pack_Status p)
{
   m_pack_status = p;

   std::ostringstream ss;
   char text[64];
   ss << "TeslaSlaveMonitor: " << m_pack_name << ": pack status set to " << getPackStatusTEXT(p);
   if (m_log) m_log->alarm(ss, __FILENAME__, __LINE__);
}

void TeslaSlaveMonitor::logStartupStatus() const
{
   if (m_pack_status == TeslaSlaveMonitor::STARTUP)
   {  
      std::string s1;
      // std::ostringstream s1;
      // s1 << "TeslaSlaveMonitor:" << m_pack_name << ": ";
      s1.append("TeslaSlaveMonitor:").append(m_pack_name).append(": ");
      // if (!m_voltages_ok) {
      //    std::ostringstream s2;
      //    s2 << s1 << logging::Hex(ID_LBC_DATA_REPLY) << " Voltages (in spec) not yet received";
      //    if (m_log) m_log->info(s2);
      //    if (CRIT_HIGH_VOLT && !isnan(m_max_cell_volts)) {
      //       std::ostringstream s3;
      //       s3 << "CRIT_HIGH_VOLT condition: voltage = " << m_max_cell_volts;
      //       if (m_log) m_log->alarm(s3);
      //    }
      //    if (CRIT_LOW_VOLT && !isnan(m_min_cell_volts)) {
      //       std::ostringstream s4;
      //       s4 << "CRIT_LOW_VOLT condition: voltage = " << m_min_cell_volts;
      //       if (m_log) m_log->alarm(s4);
      //    }
      //    if (CRIT_SPREAD_VOLT && !isnan(m_max_cell_volts) && !isnan(m_min_cell_volts)) {
      //       std::ostringstream s5;
      //       s5 << "CRIT_SPREAD_VOLT condition: voltage = " << m_max_cell_volts << " - " << m_min_cell_volts << " = " << (m_max_cell_volts-m_min_cell_volts);
      //       if (m_log) m_log->alarm(s5);
      //    }
      // }
      if (!m_temperatures_ok) {
         std::ostringstream s2;
         s2 << s1 << logging::Hex(ID_TS_TEMPS) << " Temperatures (in spec) not yet received";
         if (m_log) m_log->info(s2);
      }
      // if (!m_bat_state_recv) {
      //    std::ostringstream s2;
      //    s2 << s1 << logging::Hex(ID_LBC_DATA_REPLY) << " Battery State not yet received";
      //    if (m_log) m_log->info(s2);
      // }
      // if (!m_bat_status_recv) {
      //    std::ostringstream s2;
      //    s2 << s1 << logging::Hex(ID_BATTERY_STATUS) << " Battery Status not yet received";
      //    if (m_log) m_log->info(s2);
      // }
      // if (!m_bat_limits_recv) {
      //    std::ostringstream s2;
      //    s2 << s1 << logging::Hex(ID_LBC_POWER_LIMITS) << " Battery Limits not yet received";
      //    if (m_log) m_log->info(s2);
      // }
   }
}

float TeslaSlaveMonitor::getVoltage() const
{
   return m_voltage;
}

float TeslaSlaveMonitor::getMinCellVolts() const
{
   return m_min_cell_volts;
}

float TeslaSlaveMonitor::getMaxCellVolts() const
{
   return m_max_cell_volts;
}

float TeslaSlaveMonitor::getCurrent() const
{
   return m_current;
}

float TeslaSlaveMonitor::getTemperature() const
{
   return m_average_temperature;
}

float TeslaSlaveMonitor::getSocPercent() const
{
   return m_soc_percent;
}

float TeslaSlaveMonitor::getSohPercent() const
{
   return m_soh_percent;
}

float TeslaSlaveMonitor::getEnergyRemainingKwh() const
{
   return m_energy_remaining_kwh;
}

float TeslaSlaveMonitor::getCapacityKwh() const
{
   return m_capacity_kwh;
}

uint32_t TeslaSlaveMonitor::getSystemVersion() const
{
   return 1; // TODO - come up with a versioning scheme
}

uint32_t TeslaSlaveMonitor::getSerialNumber() const
{
   return 1; // TODO - do we configure this, or can we get it from the battery?
}

float TeslaSlaveMonitor::getNominalCapacityKwh() const
{
   return NOMINAL_CAPACITY_KWH; // TODO - configurable
}

unsigned TeslaSlaveMonitor::getNumberOfModules() const
{
   return NUM_MODULES; // TODO - configurable
}

uint32_t TeslaSlaveMonitor::getManufacturingDateUnixTime() const
{
   return 1; // TODO - where do we get this from?
}

const char* TeslaSlaveMonitor::getManufacturerName() const
{
   return "TIML-LM";
}

const char* TeslaSlaveMonitor::getBatteryName() const
{
   return "LeafG2-LM";
}

float TeslaSlaveMonitor::getMaxChargeVoltage() const
{
   return CRITICALLY_HIGH_VOLTAGE * NUM_CELLS;
}

float TeslaSlaveMonitor::getMinDischargeVoltage() const
{
   return CRITICALLY_LOW_VOLTAGE * NUM_CELLS;
}

float TeslaSlaveMonitor::getChargeCurrentLimit() const
{
   return m_charge_current_limit;
}

float TeslaSlaveMonitor::getDischargeCurrentLimit() const
{
   return m_discharge_current_limit;
}

uint32_t TeslaSlaveMonitor::getVoltTempStatus() const
{
	return m_volt_temp_status;
}

uint32_t TeslaSlaveMonitor::getFailsafeStatus() const
{
	return m_failsafe_status;
}

std::string TeslaSlaveMonitor::getAlarmConditionText() const
{

   std::string ss;
   ss.append(m_pack_name);
   ss.append(":  Alarm Condition(s) Present:\n\t\t\t\t\t");
   if (m_volt_temp_status & MAX_TEMP_MISSING) {
      ss.append("MAX_TEMP_SENSORS_MISSING\n\t\t\t\t\t");
   }
   if (m_volt_temp_status & CRIT_LOW_TEMP) {
      ss.append("CRITICALLY_LOW_TEMPERATURE\n\t\t\t\t\t");
   }
   if (m_volt_temp_status & CRIT_HIGH_TEMP) {
      ss.append("CRITICALLY_HIGH_TEMPERATURE\n\t\t\t\t\t");
   }
   if (m_volt_temp_status & CRIT_SPREAD_VOLT) {
      ss.append("CRITICALLY_HIGH_VOLTAGE_SPREAD\n\t\t\t\t\t");
   }
   if (m_volt_temp_status & CRIT_LOW_VOLT) {
      ss.append("CRITICALLY_LOW_VOLTAGE\n\t\t\t\t\t");
   }
   if (m_volt_temp_status & CRIT_HIGH_VOLT) {
      ss.append("CRITICALLY_HIGH_VOLTAGE\n\t\t\t\t\t");
   }
   return ss;
}

TeslaSlaveMonitor::CurrentLimitSmoothing::CurrentLimitSmoothing(float max) {
   m_max_current = max;

   // initialize array values
   for (int i=0; i<HIST_SIZE; i++) {
      m_hist_data[i] = 0.0;
   }

}

float TeslaSlaveMonitor::CurrentLimitSmoothing::process(float input) {
   // test for near zero, if true reset the history data to 0s
   if (input < 0.1) {
      for (int i=0; i<HIST_SIZE; i++) {
         m_hist_data[i] = 0.0;
      }
   }
   else if (input > m_max_current) {
      input = m_max_current;
   }
   // insert new data
   m_hist_data[m_hist_index] = input;
   // calculate sum
   float sum = 0;
   for (int i=0; i<HIST_SIZE; i++) {
      sum += m_hist_data[i];
   }
   // move the index
   m_hist_index = (m_hist_index+1) % HIST_SIZE;

   return (sum / HIST_SIZE);
}

}
}
