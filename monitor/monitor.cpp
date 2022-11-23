#include "Monitor.hpp"
#include "bitset"

namespace monitor {

logging::ostream& operator<<(logging::ostream& os, std::vector<monitor::Monitor*> vm)
{
   unsigned i;
   char text[20];

   os << "Pack Status           ";
   for (i=0; i<vm.size(); i++) {
      os << "   " << monitor::getPackStatusText(vm[i]->getPackStatus(), text );}
   os << std::endl;
   
   os << "Failsafe Status       ";
   for (i=0; i<vm.size(); i++) {
      os << "   " << std::bitset<3>(vm[i]->getFailsafeStatus()); }
   os << " bits" << std::endl;
   
   os << "Contactor Status      ";
   for (i=0; i<vm.size(); i++) {
      os << "   " << std::bitset<6>(vm[i]->getVoltTempStatus());}
   os << " bits" << std::endl;

   os << "Voltage:              ";
   for (i=0; i<vm.size(); i++) {
      os << "   " << vm[i]->getVoltage();}
   os << " V" << std::endl;

   os << "Current:              ";
   for (i=0; i<vm.size(); i++) {
      os << "   " << vm[i]->getCurrent();}
   os << " A" << std::endl;

   os << "Temperature:          ";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getTemperature();}
   os << " degC" << std::endl;

   os << "SOC:                  ";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getSocPercent();}
   os << " %" << std::endl;

   os << "SOH:                  ";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getSohPercent();}
   os << " %" << std::endl;

   os << "Energy Remaining:     ";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getEnergyRemainingKwh();}
   os << " kWh" << std::endl;

   os << "Capacity:             ";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getCapacityKwh();}
   os << " kWh" << std::endl;
   // os << "System Version:          " << vm[i]->getSystemVersion() << std::endl;
   // os << "Serial Number:           " << vm[i]->getSerialNumber() << std::endl;

   // os << "Nominal Capacity:     ";
   // for (i=0; i<vm.size(); i++) {
   //    os << "   "  << vm[i]->getNominalCapacityKwh();}
   // os << "kWh" << std::endl;

   // os << "Number of Modules:    ";
   // for (i=0; i<vm.size(); i++) {
   //    os << "   "  << vm[i]->getNumberOfModules();}
   // os << std::endl;
   // os << "Manufacturing Date:      " << vm[i]->getManufacturingDateUnixTime() << std::endl;
   // os << "Manufacturer Name:       " << vm[i]->getManufacturerName() << std::endl;
   // os << "Battery Name:            " << vm[i]->getBatteryName() << std::endl;

   os << "Max Charge Voltage:   ";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getMaxChargeVoltage();}
   os << " V" << std::endl;

   os << "Min Discharge Voltage:";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getMinDischargeVoltage();}
   os << " V" << std::endl;

   os << "Charge Current Limit: ";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getChargeCurrentLimit();}
   os << " A" << std::endl;

   os << "Discharge Current Lmt:";
   for (i=0; i<vm.size(); i++) {
      os << "   "  << vm[i]->getDischargeCurrentLimit();}
   os << " A" << std::endl;

   return os;
}

char* getPackStatusText(Monitor::Pack_Status p, char *text)
{
   switch (p) {
   case Monitor::Pack_Status::STARTUP:
      sprintf(text,"start");
      break;
   case Monitor::Pack_Status::NORMAL_OPERATION:
      sprintf(text,"normal");
      break;
   case Monitor::Pack_Status::SHUNT_ACTIVIATED:
      sprintf(text,"shunt_trig");
      break;
   case Monitor::Pack_Status::SHUNT_ACT_FAILED:
      sprintf(text,"shunt_fail");
      break;
   case Monitor::Pack_Status::SHUTDOWN:
      sprintf(text,"shutdown");
      break;
   default:
      sprintf(text,"invalid");
   }

   return text;
}


}
