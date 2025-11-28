#include "Monitor.hpp"
#include "bitset"
#include <iomanip>
#include <chrono>
namespace monitor
{

   logging::ostream &operator<<(logging::ostream &os, std::vector<monitor::Monitor *> vm)
   {
      unsigned i;
      char text[64];

      static auto start = std::chrono::system_clock::now();
      auto nowtime = std::chrono::system_clock::now();
      std::time_t now_time = std::chrono::system_clock::to_time_t(nowtime);
      std::chrono::duration<double> elapsed_seconds = nowtime - start;

      os << "Current Time: " << std::ctime(&now_time);
      os << "Run Time: " << std::fixed << std::setprecision(3) << (elapsed_seconds.count() / 3600.0) << " hours" << std::endl;
      os << std::defaultfloat << std::setprecision(6);   

      os << "Battery Number:       ";
      for (i = 0; i < vm.size(); i++)
      {
         if (i == (vm.size() - 1))
         {
            os << "\tall";
         }
         else
         {
            os << "\t" << i + 1;
         }
      }
      os << std::endl;

      os << "Pack Status           ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << monitor::getPackStatusText(vm[i]->getPackStatus());
      }
      os << std::endl;

      os << "Failsafe Status       ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << std::bitset<3>(vm[i]->getFailsafeStatus());
      }
      os << "\tbits" << std::endl;

      os << "Contactor Status      ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << std::bitset<6>(vm[i]->getVoltTempStatus());
      }
      os << "\tbits" << std::endl;

      std::ios::fmtflags f = os.flags();   // save flags
      os << std::fixed << std::setprecision(1);

      os << "Voltage:              ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getVoltage();
      }
      os << "\tV" << std::endl;

      os << "Current:              ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getCurrent();
      }
      os << "\tA" << std::endl;

      os << "Temperature:          ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getTemperature();
      }
      os << "\tdegC" << std::endl;

      os << "SOC:                  ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getSocPercent();
      }
      os << "\t%" << std::endl;

      os << "SOH:                  ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getSohPercent();
      }
      os << "\t%" << std::endl;

      os << "Energy Remaining:     ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getEnergyRemainingKwh();
      }
      os << "\tkWh" << std::endl;

      os << "Capacity:             ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getCapacityKwh();
      }
      os << "\tkWh" << std::endl;
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
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getMaxChargeVoltage();
      }
      os << "\tV" << std::endl;

      os << "Min Discharge Voltage:";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getMinDischargeVoltage();
      }
      os << "\tV" << std::endl;

      os.flags(f);

      os << "Charge Current Limit: ";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getChargeCurrentLimit();
      }
      os << "\tA" << std::endl;

      os << "Discharge Current Lmt:";
      for (i = 0; i < vm.size(); i++)
      {
         os << "\t" << vm[i]->getDischargeCurrentLimit();
      }
      os << "\tA" << std::endl;

      return os;
   }

   const char *getPackStatusText(Monitor::Pack_Status p)
   {
      switch (p)
      {
      case Monitor::Pack_Status::STARTUP:
         return "startup";
         break;
      case Monitor::Pack_Status::STARTUP_FAILED:
         return "strt_fl";
         break;
      case Monitor::Pack_Status::NORMAL_OPERATION:
         return "normal";
         break;
      case Monitor::Pack_Status::SHUNT_ACTIVIATED:
         return "shnt_tg";
         break;
      case Monitor::Pack_Status::SHUNT_ACT_FAILED:
         return "shnt_fl";
         break;
      case Monitor::Pack_Status::SHUTDOWN:
         return "shutdwn";
         break;
      default:
         return "invalid";
      }
   }

   const char *getPackStatusTEXT(Monitor::Pack_Status p)
   {
      switch (p)
      {
      case Monitor::Pack_Status::STARTUP:
         return "STARTUP";
         break;
      case Monitor::Pack_Status::STARTUP_FAILED:
         return "STARTUP_FAILED";
         break;
      case Monitor::Pack_Status::NORMAL_OPERATION:
         return "NORMAL_OPERATION";
         break;
      case Monitor::Pack_Status::SHUNT_ACTIVIATED:
         return "SHUNT_ACTIVIATED";
         break;
      case Monitor::Pack_Status::SHUNT_ACT_FAILED:
         return "SHUNT_ACT_FAILED";
         break;
      case Monitor::Pack_Status::SHUTTING_DOWN:
         return "SHUTTING_DOWN";
         break;
      case Monitor::Pack_Status::SHUTDOWN:
         return "SHUTDOWN";
         break;
      default:
         return "INVALID";
      }
   }

}
