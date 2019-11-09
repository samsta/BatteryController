/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Monitor.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include "contactor/Contactor.hpp"

using namespace can::messages::Nissan;

namespace monitor {
namespace Nissan {

namespace {

struct MaxMin
{
   float max;
   float min;
};

template<class Ty>
MaxMin findMaxMin(const Ty& instance, float (Ty::*get_value)(unsigned index) const, unsigned num_elements)
{
   MaxMin max_min = { (instance.*get_value)(0), (instance.*get_value)(0) };

   for (unsigned k = 1; k < num_elements; k++)
   {
      float v = (instance.*get_value)(k);
      if (v > max_min.max) max_min.max = v;
      if (v < max_min.min) max_min.min = v;
   }
   return max_min;
}

}

Monitor::Monitor(contactor::Contactor& contactor):
      m_contactor(contactor),
      m_voltages_ok(false),
      m_temperatures_ok(false),
      m_everything_ok(false)
{
   m_contactor.setSafeToOperate(false);
}

void Monitor::process(const CellVoltages& voltages)
{
   if (not voltages.valid()) return;

   MaxMin voltage = findMaxMin(voltages, &CellVoltages::getVoltage, voltages.NUM_CELLS);

   if (voltage.max < 4.15 &&
       voltage.min > 3    &&
       (voltage.max - voltage.min) < 0.1)
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

   MaxMin temperature = findMaxMin(temperatures, &PackTemperatures::getTemperature, temperatures.NUM_PACKS);

   if (temperature.max < 50 &&
       temperature.min > 2)
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
