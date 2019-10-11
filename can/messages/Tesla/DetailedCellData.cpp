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
         m_values[k] = frame.getBitField(bit_index, 14) * VOLT_PER_UNIT;
      }
      else
      {
         m_values[k] = frame.getSignedBitField(bit_index, 14) * DEGC_PER_UNIT;
      }
   }
   m_valid = true;
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

}
}
}
