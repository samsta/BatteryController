/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MONITOR_MONITOR_HPP
#define _MONITOR_MONITOR_HPP

#include <stdint.h>
#include "logging/stream.hpp"

namespace monitor {

class Monitor
{
public:
   virtual ~Monitor(){};

   virtual float getVoltage() const = 0;
   virtual float getCurrent() const = 0;
   virtual float getTemperature() const = 0;
   virtual float getSocPercent() const = 0;
   virtual float getSohPercent() const = 0;
   virtual float getEnergyRemainingKwh() const = 0;
   virtual float getCapacityKwh() const = 0;
   
   virtual uint32_t getSystemVersion() const = 0;
   virtual uint32_t getSerialNumber() const = 0;
   virtual float getNominalCapacityKwh() const = 0;
   virtual unsigned getNumberOfModules() const = 0;
   virtual uint32_t getManufacturingDateUnixTime() const = 0;
   virtual const char* getManufacturerName() const = 0;
   virtual const char* getBatteryName() const = 0;

   virtual float getMaxChargeVoltage() const = 0;
   virtual float getMinDischargeVoltage() const = 0;
   virtual float getChargeCurrentLimit() const = 0;
   virtual float getDischargeCurrentLimit() const = 0;

   virtual uint32_t getContactorStatus() const = 0;
};

logging::ostream& operator<<(logging::ostream& os, const Monitor& monitor);

}

#endif // _MONITOR_MONITOR_HPP
