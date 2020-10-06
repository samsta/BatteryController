/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MONITOR_MONITOR_HPP
#define _MONITOR_MONITOR_HPP

namespace monitor {

class Monitor
{
public:
   virtual ~Monitor(){};

   virtual float getSocPercent() const = 0;
   virtual float getSohPercent() const = 0;
   virtual float getEnergyRemainingKwh() const = 0;
   virtual float getFullChargedEnergyKwh() const = 0;
   
};

}

#endif // _MONITOR_MONITOR_HPP
