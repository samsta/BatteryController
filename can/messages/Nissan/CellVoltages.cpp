/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "CellVoltages.hpp"
#include "logging/Hex.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {
namespace {

const float VOLTS_PER_UNIT = 0.001;
const unsigned GROUP_SIZE = 198;

}

CellVoltages::CellVoltages(const DataFrame& f): Message(ID_LBC_DATA_REPLY, GROUP_CELL_VOLTAGES)
{
   if (f.id() != id()) return;
   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != dataGroup()) return;

   for (unsigned k = 0; k < NUM_CELLS; k++)
   {
      m_voltages[k] = VOLTS_PER_UNIT * f.getUnsignedShort((k + 1) * 2);
   }

   setValid();
}

CellVoltages::CellVoltages(): Message(ID_LBC_DATA_REPLY, GROUP_CELL_VOLTAGES)
{
}

float CellVoltages::getVoltage(unsigned cell_index) const
{
   if (cell_index >= NUM_CELLS) return NAN;
   return m_voltages[cell_index];
}

CellVoltages& CellVoltages::setVoltage(unsigned cell_index, float voltage)
{
   if (cell_index >= NUM_CELLS) return *this;
   setValid();
   m_voltages[cell_index] = voltage;
   return *this;
}

void CellVoltages::toStream(logging::ostream& os) const
{
   os << "CellVoltages: " << logging::Hex(ID_LBC_DATA_REPLY) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }
   for (unsigned k = 0; k < CellVoltages::NUM_CELLS; k++)
   {
      if (k != 0)
      {
         os << ", ";
      }
      os << getVoltage(k) << 'V';
   }
}

}
}
}
