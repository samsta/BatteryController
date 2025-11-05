/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "TSCellVoltages.hpp"
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

TSCellVoltages::TSCellVoltages():
   Message(ID_TS_CELL_VOLT, GROUP_NONE),
   m_min_cell_voltage(),
   m_max_cell_voltage()
{
}

TSCellVoltages::TSCellVoltages(const DataFrame& frame):
   Message(ID_TS_CELL_VOLT, GROUP_NONE),
   m_min_cell_voltage(),
   m_max_cell_voltage()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   m_max_cell_voltage = frame.getSignedShort(0) / 100.0f;
   m_min_cell_voltage = frame.getSignedShort(2) / 100.0f;
   
   setValid();
}

float TSCellVoltages::getMaxCellVoltage() const
{
   return m_max_cell_voltage;
}

float TSCellVoltages::getMinCellVoltage() const
{
   return m_min_cell_voltage;
}

void TSCellVoltages::toStream(logging::ostream& os) const
{
   os << "TSCellVoltages: 0x" << logging::Hex(ID_TS_TEMPS) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "MinTemp=" << m_min_cell_voltage << "V"
      << "MaxTemp=" << m_max_cell_voltage << "V";
}


}
}
}




