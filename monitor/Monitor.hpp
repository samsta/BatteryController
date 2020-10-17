/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MONITOR_MONITOR_HPP
#define _MONITOR_MONITOR_HPP

#include <stdint.h>

namespace monitor {

class Monitor
{
public:
   virtual ~Monitor(){};

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
};

}

#endif // _MONITOR_MONITOR_HPP
