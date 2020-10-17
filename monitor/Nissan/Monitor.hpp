/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MONITOR_NISSAN_MONITOR_HPP
#define _MONITOR_NISSAN_MONITOR_HPP

#include "monitor/Monitor.hpp"
#include "can/FrameSink.hpp"

namespace contactor {
class Contactor;
}

namespace can {
namespace messages {
namespace Nissan {
class Message;
class CellVoltageRange;
class PackTemperatures;
class BatteryState;
}
}
}

namespace monitor {
namespace Nissan {

class Monitor: public monitor::Monitor
{
public:
   explicit Monitor(contactor::Contactor&);

   void sink(const can::messages::Nissan::Message&);

   // monitor::Monitor
   virtual float getSocPercent() const;
   virtual float getSohPercent() const;
   virtual float getEnergyRemainingKwh() const;
   virtual float getCapacityKwh() const;

   virtual uint32_t getSystemVersion() const;
   virtual uint32_t getSerialNumber() const;
   virtual float getNominalCapacityKwh() const;
   virtual unsigned getNumberOfModules() const;
   virtual uint32_t getManufacturingDateUnixTime() const;
   virtual const char* getManufacturerName() const;
   virtual const char* getBatteryName() const;
   
private:
   void process(const can::messages::Nissan::CellVoltageRange&);
   void process(const can::messages::Nissan::PackTemperatures&);
   void process(const can::messages::Nissan::BatteryState&);
   void updateOperationalSafety();

   contactor::Contactor& m_contactor;
   bool m_voltages_ok;
   bool m_temperatures_ok;
   bool m_everything_ok;

   float m_soc_percent;
   float m_soh_percent;
   float m_energy_remaining_kwh;
   float m_capacity_kwh;
};

}
}

#endif // _MONITOR_NISSAN_MONITOR_HPP
