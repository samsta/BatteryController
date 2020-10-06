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
   MOCK_METHOD(float, getFullChargedEnergyKwh, (), (const));
};

}
}

#endif // _MOCKS_MONITOR_MONITOR_HPP
