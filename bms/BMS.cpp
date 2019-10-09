
#include "BMS.hpp"

namespace {
   const float VOLTS_PER_UNIT = 0.000305;
   const float DEGC_PER_UNIT = 0.0122;
}

BMS::BMS():
   m_cell_voltages(),
   m_temperatures()
{	
}

void BMS::receive(const can::DataFrame& frame)
{
   unsigned msg_index = frame.data[0];
   bool is_voltage = msg_index < 24;
   for (unsigned k = 0, bit_index = 8; k < 4; k++, bit_index += 14)
   {
      if (is_voltage)
      {
         m_cell_voltages[msg_index * 4 + k] = frame.getBitField(bit_index, 14) * VOLTS_PER_UNIT;
      }
      else
      {
         m_temperatures[(msg_index - 24) * 4 + k] = frame.getSignedBitField(bit_index, 14) * DEGC_PER_UNIT;
      }
   }
}

float BMS::getVoltage()  
{
   float voltage = 0;
   for (unsigned i = 0; i < NUM_MODULES; i++)
   {
      voltage += getModuleVoltage(i);
   }
   return voltage;
}

float BMS::getCellVoltage(unsigned index)  
{  
   if (index >= NUM_CELLS) return -1;

   return m_cell_voltages[index];
}

float BMS::getTemperature(unsigned index)  
{  
   if (index >= NUM_TEMPERATURE_SENSORS) return -1;

   return m_temperatures[index];
}

float BMS::getModuleVoltage(unsigned index)  
{  
   if (index >= NUM_MODULES) return -1;
  
   float voltage = 0;
   for (unsigned i = index * NUM_CELLS_PER_MODULE; i < (index + 1) * NUM_CELLS_PER_MODULE; i++)
   {
      voltage += getCellVoltage(i);
   }
   return voltage;
}
