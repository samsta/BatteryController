/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Monitor.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "contactor/Contactor.hpp"
#include <math.h>

using namespace can::messages::Nissan;

namespace monitor {
namespace Nissan {

namespace {

const float CRITICALLY_HIGH_VOLTAGE(4.15);
const float CRITICALLY_LOW_VOLTAGE(3);
const float CRITICALLY_HIGH_VOLTAGE_SPREAD(0.1);

const float CRITICALLY_HIGH_TEMPERATURE(50);
const float CRITICALLY_LOW_TEMPERATURE(2);
const float MAX_TEMP_SENSORS_MISSING(1);

const float NOMINAL_CAPACITY_KWH(24);
const unsigned NUM_MODULES(24);

}

Monitor::Monitor(contactor::Contactor& contactor):
      m_contactor(contactor),
      m_voltages_ok(false),
      m_temperatures_ok(false),
      m_everything_ok(false),
      m_soc_percent(NAN),
      m_soh_percent(NAN),
      m_energy_remaining_kwh(NAN),
      m_capacity_kwh(NAN)
{
   m_contactor.setSafeToOperate(false);
}

void Monitor::sink(const can::messages::Nissan::Message& message)
{
   if (not message.valid()) return;

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
      return;
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
   updateOperationalSafety();
}

void Monitor::process(const can::messages::Nissan::PackTemperatures& temperatures)
{
   unsigned num_sensors_missing = 0;
   float max_temp = NAN, min_temp;
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

      if (isnan(temp)) num_sensors_missing++;
   }

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
   updateOperationalSafety();
}

void Monitor::process(const can::messages::Nissan::BatteryState& battery_state)
{
   m_soc_percent = battery_state.getSocPercent();
   m_soh_percent = battery_state.getHealthPercent();
   m_capacity_kwh = (NOMINAL_CAPACITY_KWH/100) * m_soh_percent;
   m_energy_remaining_kwh = (m_capacity_kwh/100) * m_soc_percent;
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
   return "Nissan";
}

const char* Monitor::getBatteryName() const
{
   return "Leaf 24kWh"; // TODO - configurable
}




}
}
