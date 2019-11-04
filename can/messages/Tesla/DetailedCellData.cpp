/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "DetailedCellData.hpp"
#include "can/DataFrame.hpp"
#include <math.h>

namespace can {
namespace messages {
namespace Tesla {

namespace {
const unsigned NUM_VOLTAGE_MESSAGES = 24;
const unsigned NUM_TEMPERATURE_MESSAGES = 8;
const unsigned FIRST_TEMPERATURE_INDEX = NUM_VOLTAGE_MESSAGES;
const unsigned MAX_INDEX = FIRST_TEMPERATURE_INDEX + NUM_TEMPERATURE_MESSAGES - 1;
const unsigned VALUES_PER_FRAME = 4;
const float VOLT_PER_UNIT = 0.0008;
const float DEGC_PER_UNIT = 0.01;
}

DetailedCellData::DetailedCellData(const DataFrame& frame):
      m_base_index(),
      m_is_voltage(),
      m_values(),
      m_valid(false)
{
   if (frame.id() != 0x6f2) return;

   unsigned msg_index = frame.data()[0];

   if (frame.size() != 8) return;
   if (msg_index > MAX_INDEX) return;

   m_is_voltage = msg_index < FIRST_TEMPERATURE_INDEX;
   if (m_is_voltage)
   {
      m_base_index = msg_index * VALUES_PER_FRAME;
   }
   else
   {
      m_base_index = (msg_index - FIRST_TEMPERATURE_INDEX) * VALUES_PER_FRAME;
   }

   for (unsigned k = 0, bit_index = 8; k < 4; k++, bit_index += 14)
   {
      if (m_is_voltage)
      {
         m_values[k] = frame.getBitField(bit_index, 13) * VOLT_PER_UNIT;
      }
      else
      {
         m_values[k] = frame.getSignedBitField(bit_index, 13) * DEGC_PER_UNIT;
      }
   }
   m_valid = true;
}

DetailedCellData::DetailedCellData():
      m_base_index(),
      m_is_voltage(),
      m_values(),
      m_valid(false)
{
}

bool DetailedCellData::isVoltage() const
{
   return m_is_voltage;
}

float DetailedCellData::getValue(unsigned index) const
{
   if (index >= VALUES_PER_FRAME) return NAN;
   return m_values[index];
}

unsigned DetailedCellData::getBaseIndex() const
{
   return m_base_index;
}

bool DetailedCellData::valid() const
{
   return m_valid;
}

DetailedCellData& DetailedCellData::setIsVoltage()
{
   m_valid = true;
   m_is_voltage = true;
   return *this;
}

DetailedCellData& DetailedCellData::setIsTemperature()
{
   m_valid = true;
   m_is_voltage = false;
   return *this;
}

DetailedCellData& DetailedCellData::setValue(unsigned index, float value)
{
   if (index < VALUES_PER_FRAME)
   {
      m_valid = true;
      m_values[index] = value;
   }
   return *this;
}

DetailedCellData& DetailedCellData::setBaseIndex(unsigned index)
{
   m_valid = true;
   m_base_index = index;
   return *this;
}

logging::ostream& operator<<(logging::ostream& os, const DetailedCellData& d)
{
   os << "DetailedCellData: ";
   if (not d.valid()) return os << "invalid";

   const char symbol = d.isTemperature() ? 'T' : 'U';
   const char* unit =  d.isTemperature() ? "degC" : "V";

   for (unsigned k = 0; k < VALUES_PER_FRAME; k++)
   {
      if (k != 0)
      {
         os << ", ";
      }
      os << symbol << k + d.getBaseIndex() << "=" << d.getValue(k) << unit;
   }
   return os;
}


}
}
}
