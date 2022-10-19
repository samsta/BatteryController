#include "Monitor.hpp"
#include "bitset"

namespace monitor {

logging::ostream& operator<<(logging::ostream& os, const Monitor& m)
{
   os << "Failsafe Status          " << std::bitset<3>(m.getFailsafeStatus()) << " bits (" << m.getFailsafeStatus() << ")" << std::endl;
   os << "Contactor Status         " << std::bitset<6>(m.getContactorStatus()) << " bits" << std::endl;
   os << "Voltage:                 " << m.getVoltage() << " V" << std::endl;
   os << "Current:                 " << m.getCurrent() << " A" << std::endl;
   os << "Temperature:             " << m.getTemperature() << " degC" << std::endl;
   os << "SOC:                     " << m.getSocPercent() << " %" << std::endl;
   os << "SOH:                     " << m.getSohPercent()<< " %" << std::endl;
   os << "Energy Remaining:        " << m.getEnergyRemainingKwh() << " kWh" << std::endl;
   os << "Capacity:                " << m.getCapacityKwh() << " kWh" << std::endl;
   os << "System Version:          " << m.getSystemVersion() << std::endl;
   os << "Serial Number:           " << m.getSerialNumber() << std::endl;
   os << "Nominal Capacity:        " << m.getNominalCapacityKwh() << "kWh" << std::endl;
   os << "Number of Modules:       " << m.getNumberOfModules() << std::endl;
   os << "Manufacturing Date:      " << m.getManufacturingDateUnixTime() << std::endl;
   os << "Manufacturer Name:       " << m.getManufacturerName() << std::endl;
   os << "Battery Name:            " << m.getBatteryName() << std::endl;
   os << "Max Charge Voltage:      " << m.getMaxChargeVoltage() << " V" << std::endl;
   os << "Min Discharge Voltage:   " << m.getMinDischargeVoltage() << " V" << std::endl;
   os << "Charge Current Limit:    " << m.getChargeCurrentLimit() << " A" << std::endl;
   os << "Discharge Current Limit: " << m.getDischargeCurrentLimit() << " A" << std::endl;

   return os;
}

}
