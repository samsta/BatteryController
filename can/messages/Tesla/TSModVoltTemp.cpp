/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "TSModVoltTemp.hpp"
#include "logging/Hex.hpp"
#include "Ids.hpp"
#include "can/DataFrame.hpp"
#include <stdint.h>
#include <math.h>

namespace can {
namespace messages {
namespace Tesla {
namespace {

}

TSModVoltTemp::TSModVoltTemp():
   Message(ID_TS_MOD_VOLT_TEMP, GROUP_NONE),
   m_volt_temp_data()
{
}

TSModVoltTemp::TSModVoltTemp(const DataFrame& frame):
   Message(ID_TS_MOD_VOLT_TEMP, GROUP_NONE),
   m_volt_temp_data()
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   m_volt_temp_data.module_num = frame.getByte(1);
   m_volt_temp_data.voltage = frame.getSignedShort(2) / 100.0f;
   m_volt_temp_data.temperatures[0] = frame.getSignedShort(4) / 100.0f;
   m_volt_temp_data.temperatures[1] = frame.getSignedShort(6) / 100.0f;
   setValid();
}

void TSModVoltTemp::toStream(logging::ostream& os) const
{
   os << "TSModVoltTemp: 0x" << logging::Hex(ID_TS_MOD_VOLT_TEMP) << " ";

   if (not valid())
   {
      os << "invalid";
      return;
   }

   os << "ModNum= " << m_volt_temp_data.module_num;
}


}
}
}
