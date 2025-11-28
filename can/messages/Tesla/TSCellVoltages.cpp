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

TSVoltages::TSVoltages():
   Message(ID_TS_CELL_VOLT_CUR, GROUP_NONE),
   m_min_cell_voltage(),
   m_max_cell_voltage(),
   m_pack_voltage()
{
}

TSVoltages::TSVoltages(const DataFrame& frame):
   Message(ID_TS_CELL_VOLT_CUR, GROUP_NONE),
   m_min_cell_voltage(),
   m_max_cell_voltage(),
   m_pack_voltage()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   m_max_cell_voltage = frame.getUnsignedShort(0) / 100.0f;
   m_min_cell_voltage = frame.getUnsignedShort(2) / 100.0f;
   m_pack_voltage     = frame.getUnsignedShort(4) / 64.0f;
   m_dc_current       = frame.getSignedShort(6) / 32.0f;
   setValid();
}

float TSVoltages::getMaxCellVoltage() const
{
   return m_max_cell_voltage;
}

float TSVoltages::getMinCellVoltage() const
{
   return m_min_cell_voltage;
}

float TSVoltages::getPackVoltage() const
{
   return m_pack_voltage;
}  

float TSVoltages::getDCCurent() const
{
   return m_dc_current;
}

void TSVoltages::toStream(logging::ostream& os) const
{
   os << "TSVoltages: 0x" << logging::Hex(ID_TS_CELL_VOLT_CUR) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "MinCellV= " << m_min_cell_voltage << " V "
      << "MaxCellV= " << m_max_cell_voltage << " V "
      << "PackV= "     << m_pack_voltage     << " V";
}


}
}
}




