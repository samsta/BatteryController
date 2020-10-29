/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "CellVoltageRange.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Nissan {

namespace {

const unsigned BYTE_OFFSET_MAX_VOLTAGE = 12;
const unsigned BYTE_OFFSET_MIN_VOLTAGE = 14;
const float VOLTS_PER_UNIT = 0.001;
const unsigned GROUP_SIZE = 28;

}

CellVoltageRange::CellVoltageRange(const DataFrame& f):
   Message(ID_LBC_DATA_REPLY, GROUP_CELL_VOLTAGE_RANGE),
   m_min_voltage(NAN),
   m_max_voltage(NAN)
{
   if (f.id() != id()) return;
   if (f.size() != GROUP_SIZE) return;
   if (f.data()[1] != dataGroup()) return;
   m_max_voltage = f.getUnsignedShort(BYTE_OFFSET_MAX_VOLTAGE) * VOLTS_PER_UNIT;
   m_min_voltage = f.getUnsignedShort(BYTE_OFFSET_MIN_VOLTAGE) * VOLTS_PER_UNIT;
   setValid();
}

CellVoltageRange::CellVoltageRange():
   Message(ID_LBC_DATA_REPLY, GROUP_CELL_VOLTAGE_RANGE),
   m_min_voltage(NAN),
   m_max_voltage(NAN)
{
}

float CellVoltageRange::getMin() const
{
   return m_min_voltage;
}

CellVoltageRange& CellVoltageRange::setMin(float voltage)
{
   m_min_voltage = voltage;
   setValid();
   return *this;
}

float CellVoltageRange::getMax() const
{
   return m_max_voltage;
}

CellVoltageRange& CellVoltageRange::setMax(float voltage)
{
   m_max_voltage = voltage;
   setValid();
   return *this;
}

logging::ostream& operator<<(logging::ostream& os, const CellVoltageRange& v)
{
   os << "CellVoltageRange: ";
   if (v.valid())
   {
      os << "min=" << v.getMin() << "V ";
      os << "max=" << v.getMax() << "V";
   }
   else
   {
      os << "invalid";
   }
   return os;
}

}
}
}
