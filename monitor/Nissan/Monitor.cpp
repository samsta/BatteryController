/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Monitor.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
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

}

Monitor::Monitor(contactor::Contactor& contactor):
      m_contactor(contactor),
      m_voltages_ok(false),
      m_temperatures_ok(false),
      m_everything_ok(false)
{
   m_contactor.setSafeToOperate(false);
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
   if (not temperatures.valid()) return;

   unsigned num_sensors_missing = 0;
   float max_temp = NAN, min_temp;
   for (unsigned k = 0; k < temperatures.NUM_SENSORS; k++)
   {
      float temp = temperatures.getTemperature(k);

      if (isnanf(max_temp))
      {
         min_temp = max_temp = temp;
      }
      else
      {
         if (temp > max_temp) max_temp = temp;
         if (temp < min_temp) min_temp = temp;
      }

      if (isnanf(temp)) num_sensors_missing++;
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

void Monitor::updateOperationalSafety()
{
   bool everything_ok = m_voltages_ok && m_temperatures_ok;
   if (everything_ok != m_everything_ok)
   {
      m_contactor.setSafeToOperate(everything_ok);
   }
   m_everything_ok = everything_ok;
}

}
}
