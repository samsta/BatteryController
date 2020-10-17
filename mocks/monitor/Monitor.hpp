/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MOCKS_MONITOR_MONITOR_HPP
#define _MOCKS_MONITOR_MONITOR_HPP

namespace mocks {
namespace monitor {

class Monitor: public ::monitor::Monitor
{
public:

   MOCK_METHOD(float, getSocPercent, (), (const));
   MOCK_METHOD(float, getSohPercent, (), (const));
   MOCK_METHOD(float, getEnergyRemainingKwh, (), (const));
   MOCK_METHOD(float, getCapacityKwh, (), (const));

   MOCK_METHOD(uint32_t, getSystemVersion, (),  (const));
   MOCK_METHOD(uint32_t, getSerialNumber, (), (const));
   MOCK_METHOD(float, getNominalCapacityKwh, (), (const));
   MOCK_METHOD(unsigned, getNumberOfModules, (), (const));
   MOCK_METHOD(uint32_t, getManufacturingDateUnixTime, (), (const));
   MOCK_METHOD(const char*, getManufacturerName, (), (const));
   MOCK_METHOD(const char*, getBatteryName, (), (const));

};

}
}

#endif // _MOCKS_MONITOR_MONITOR_HPP
