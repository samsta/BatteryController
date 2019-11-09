/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MONITOR_NISSAN_MONITOR_HPP
#define _MONITOR_NISSAN_MONITOR_HPP

namespace contactor {
class Contactor;
}

namespace can {
namespace messages {
namespace Nissan {
class CellVoltages;
class PackTemperatures;
}
}
}

namespace monitor {
namespace Nissan {

class Monitor
{
public:
   explicit Monitor(contactor::Contactor&);

   void process(const can::messages::Nissan::CellVoltages&);
   void process(const can::messages::Nissan::PackTemperatures&);

private:
   void updateOperationalSafety();

   contactor::Contactor& m_contactor;
   bool m_voltages_ok;
   bool m_temperatures_ok;
   bool m_everything_ok;
};

}
}

#endif // _MONITOR_NISSAN_MONITOR_HPP
