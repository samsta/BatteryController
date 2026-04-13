/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "TSModCellVoltages.hpp"
#include "logging/Hex.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Tesla {
namespace {

}

TSModCellVoltages::TSModCellVoltages():
   Message(ID_TS_MOD_CELLS_VOLTS, GROUP_NONE),
   m_cell_voltage(),
   m_module_num()
{
}

TSModCellVoltages::TSModCellVoltages(const DataFrame& frame):
   Message(ID_TS_MOD_CELLS_VOLTS, GROUP_NONE),
   m_cell_voltage(),
   m_module_num()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   m_module_num = frame.getByte(1);

   for (int i=0; i < 6; i++)
   {
      m_cell_voltage[i] = 2.0 + (frame.getByte(i+2) / 100.0f);
   }
   setValid();
}

float TSModCellVoltages::getCellVoltage(uint cell) const
{
   return m_cell_voltage[cell];
}

uint16_t TSModCellVoltages::getModuleNum() const
{
   return m_module_num;
}


void TSModCellVoltages::toStream(logging::ostream& os) const
{
   os << "TSModCellVoltages: 0x" << logging::Hex(ID_TS_MOD_CELLS_VOLTS) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "ModNum= " << m_module_num << " V ";
      // << "MaxCellV= " << m_max_cell_voltage << " V "
      // << "PackV= "     << m_pack_voltage     << " V";
}


}
}
}
