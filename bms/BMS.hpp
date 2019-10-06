#ifndef _BMS_HPP
#define _BMS_HPP

#include <stdint.h>
#include "can/DataFrame.hpp"

class BMS
{
public:
   BMS();
	
   void receive(const can::DataFrame& frame);
   
   float getVoltage();
   float getModuleVoltage(unsigned index);
   float getCellVoltage(unsigned index);
   float getTemperature(unsigned index);

   enum Sizes {
      NUM_MODULES = 16,
      NUM_TEMPERATURE_SENSORS = 32,
      NUM_CELLS_PER_MODULE = 6,
      NUM_CELLS = NUM_CELLS_PER_MODULE * NUM_MODULES
   };

private:   
   float m_cell_voltages[NUM_CELLS];
   float m_temperatures[NUM_TEMPERATURE_SENSORS];  
};

#endif 
