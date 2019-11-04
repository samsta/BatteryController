/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "CellVoltageRange.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {

// TODO: get a CAN trace!!!

CellVoltageRange::CellVoltageRange(const DataFrame& f)
{
   if (f.id() != ID_LBC_DATA_REPLY) return;
//   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != GROUP_CELL_VOLTAGE_RANGE) return;
#ifdef HAS_STD_IOSTREAM
   std::cout << "CellVoltageRange size " << f.size();
   std::cout << " vmax " << f.getUnsignedShort(12) * 0.001;
   std::cout << " vmin " << f.getUnsignedShort(14) * 0.001;
   std::cout << std::endl;
#endif
}

bool CellVoltageRange::valid() const
{
   return false;
}

float CellVoltageRange::getMin() const
{
   return NAN;
}

float CellVoltageRange::getMax() const
{
   return NAN;
}

logging::ostream& operator<<(logging::ostream& os, const CellVoltageRange&)
{
   return os;
}

}
}
}
