/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Monitor.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "contactor/Contactor.hpp"
#include "can/messages/Nissan/BatteryPowerLimits.hpp"
#include <math.h>

using namespace can::messages::Nissan;

namespace monitor {
namespace Nissan {

namespace {

const float CRITICALLY_HIGH_VOLTAGE(4.15);
const float WARN_HIGH_VOLTAGE(4.1);
const float WARN_LOW_VOLTAGE(3.3);
const float CRITICALLY_LOW_VOLTAGE(3);
const float CRITICALLY_HIGH_VOLTAGE_SPREAD(0.1);

const float CRITICALLY_HIGH_TEMPERATURE(50);
const float WARN_HIGH_TEMPERATURE(40);
const float WARN_LOW_TEMPERATURE(5);
const float CRITICALLY_LOW_TEMPERATURE(2);
const float MAX_TEMP_SENSORS_MISSING(1);

const float NOMINAL_CAPACITY_KWH(24);
const float NOMINAL_CURRENT_LIMIT(20);

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

Monitor::Monitor(contactor::Contactor& contactor):
      m_contactor(contactor),
      m_voltages_ok(false),
      m_temperatures_ok(false),
      m_everything_ok(false),
      m_soc_percent(NAN),
      m_soh_percent(NAN),
      m_energy_remaining_kwh(NAN),
      m_capacity_kwh(NAN),
      m_current(NAN),
      m_voltage(NAN),
      m_cur_fac_by_temperature(0),
      m_charge_cur_fac_by_voltage(0),
      m_discharge_cur_fac_by_voltage(0)
{
   m_contactor.setSafeToOperate(false);
}

void Monitor::sink(const can::messages::Nissan::Message& message)
{
   if (not message.valid()) return;

   switch(message.id())
   {
   case ID_BATTERY_STATUS:
      process(static_cast<const BatteryStatus&>(message));
      break;
//   case ID_LBC_POWER_LIMITS:
//      process(static_cast<const BatteryPowerLimits&>(message));
//      break;
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

void Monitor::process(const CellVoltageRange& voltage_range)
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
   calculateCurrentLimitByVoltage(voltage_range.getMin(), voltage_range.getMax());
   updateOperationalSafety();
}

void Monitor::process(const PackTemperatures& temperatures)
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

   calculateTemperatureLimitFactor(min_temp, max_temp);
   updateOperationalSafety();
}

void Monitor::process(const BatteryState& battery_state)
{
   m_soc_percent = battery_state.getSocPercent();
   m_soh_percent = battery_state.getHealthPercent();
   m_capacity_kwh = (NOMINAL_CAPACITY_KWH/100) * m_soh_percent;
   m_energy_remaining_kwh = (m_capacity_kwh/100) * m_soc_percent;
}

void Monitor::process(const BatteryStatus& battery_status)
{
   m_current = battery_status.getCurrent();
   m_voltage = battery_status.getVoltage();
}

//void Monitor::process(const BatteryPowerLimits& battery_power)
//{
//   m_discharge_power_limit = battery_power.getDischargePowerLimit();
//   m_charge_power_limit = battery_power.getChargePowerLimit();
//}

void Monitor::calculateTemperatureLimitFactor(float min_temp, float max_temp)
{
   if (max_temp < CRITICALLY_HIGH_TEMPERATURE &&
       min_temp > CRITICALLY_LOW_TEMPERATURE)
   {
      if (max_temp >= WARN_HIGH_TEMPERATURE)
      {
         m_cur_fac_by_temperature = upper_limit(max_temp,
                                                WARN_HIGH_TEMPERATURE,
                                                CRITICALLY_HIGH_TEMPERATURE,
                                                TEMPERATURE_LIMIT_RESOLUTION);
      }
      else if (min_temp <= WARN_LOW_TEMPERATURE)
      {
         m_cur_fac_by_temperature = lower_limit(min_temp,
                                                WARN_LOW_TEMPERATURE,
                                                CRITICALLY_LOW_TEMPERATURE,
                                                TEMPERATURE_LIMIT_RESOLUTION);
      }
      else
      {
         m_cur_fac_by_temperature = 1;
      }
   }
   else
   {
      m_cur_fac_by_temperature = 0;
   }
}

void Monitor::calculateCurrentLimitByVoltage(float min_voltage, float max_voltage)
{
   if (max_voltage > CRITICALLY_HIGH_VOLTAGE)
   {
      m_charge_cur_fac_by_voltage = 0;
   }
   else if (max_voltage >= WARN_HIGH_VOLTAGE)
   {
      m_charge_cur_fac_by_voltage = upper_limit(max_voltage,
                                                WARN_HIGH_VOLTAGE,
                                                CRITICALLY_HIGH_VOLTAGE,
                                                VOLTAGE_LIMIT_RESOLUTION);
   }
   else
   {
      m_charge_cur_fac_by_voltage = 1;
   }

   if (min_voltage < CRITICALLY_LOW_VOLTAGE)
   {
      m_discharge_cur_fac_by_voltage = 0;
   }
   else if (min_voltage <= WARN_LOW_VOLTAGE)
   {
      m_discharge_cur_fac_by_voltage = lower_limit(min_voltage,
                                                   WARN_LOW_VOLTAGE,
                                                   CRITICALLY_LOW_VOLTAGE,
                                                   VOLTAGE_LIMIT_RESOLUTION);
   }
   else
   {
      m_discharge_cur_fac_by_voltage = 1;
   }
}


void Monitor::updateOperationalSafety()
{
   bool everything_ok = m_voltages_ok && m_temperatures_ok;
   if (everything_ok != m_everything_ok)
   {
      m_contactor.setSafeToOperate(everything_ok);
   }
   m_everything_ok = everything_ok;
}

float Monitor::getVoltage() const
{
   return m_voltage;
}

float Monitor::getCurrent() const
{
   return m_current;
}

float Monitor::getTemperature() const
{
   return m_average_temperature;
}

float Monitor::getSocPercent() const
{
   return m_soc_percent;
}

float Monitor::getSohPercent() const
{
   return m_soh_percent;
}

float Monitor::getEnergyRemainingKwh() const
{
   return m_energy_remaining_kwh;
}

float Monitor::getCapacityKwh() const
{
   return m_capacity_kwh;
}

uint32_t Monitor::getSystemVersion() const
{
   return 1; // TODO - come up with a versioning scheme
}

uint32_t Monitor::getSerialNumber() const
{
   return 1; // TODO - do we configure this, or can we get it from the battery?
}

float Monitor::getNominalCapacityKwh() const
{
   return NOMINAL_CAPACITY_KWH; // TODO - configurable
}

unsigned Monitor::getNumberOfModules() const
{
   return NUM_MODULES; // TODO - configurable
}

uint32_t Monitor::getManufacturingDateUnixTime() const
{
   return 0; // TODO - where do we get this from?
}

const char* Monitor::getManufacturerName() const
{
   return "LG CHEM";
}

const char* Monitor::getBatteryName() const
{
   return "RESU10H";
}

float Monitor::getMaxChargeVoltage() const
{
   return CRITICALLY_HIGH_VOLTAGE * NUM_CELLS;
}

float Monitor::getMinDischargeVoltage() const
{
   return CRITICALLY_LOW_VOLTAGE * NUM_CELLS;
}

float Monitor::getChargeCurrentLimit() const
{
   return m_charge_cur_fac_by_voltage * m_cur_fac_by_temperature * NOMINAL_CURRENT_LIMIT;
}

float Monitor::getDischargeCurrentLimit() const
{
   return m_discharge_cur_fac_by_voltage * m_cur_fac_by_temperature * NOMINAL_CURRENT_LIMIT;
}


}
}
