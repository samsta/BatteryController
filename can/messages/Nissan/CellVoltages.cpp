/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "CellVoltages.hpp"
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

CellVoltages::CellVoltages(const DataFrame& f): m_valid(false)
{
   if (f.id() != ID_LBC_DATA_REPLY) return;
   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != GROUP_CELL_VOLTAGES) return;

   for (unsigned k = 0; k < NUM_CELLS; k++)
   {
      m_voltages[k] = VOLTS_PER_UNIT * f.getUnsignedShort((k + 1) * 2);
   }

   m_valid = true;
}

bool CellVoltages::valid() const
{
   return m_valid;
}

float CellVoltages::getVoltage(unsigned cell_index) const
{
   if (cell_index >= NUM_CELLS) return NAN;
   return m_voltages[cell_index];
}

logging::ostream& operator<<(logging::ostream& os, const CellVoltages& voltages)
{
   os << "CellVoltages: ";

   if (not voltages.valid()) return os << "invalid";

   for (unsigned k = 0; k < CellVoltages::NUM_CELLS; k++)
   {
      if (k != 0)
      {
         os << ", ";
      }
      os << voltages.getVoltage(k) << 'V';
   }
   return os;
}

}
}
}
