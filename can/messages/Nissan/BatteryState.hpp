/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_NISSAN_BATTERYSTATE_HPP
#define _CAN_MESSAGES_NISSAN_BATTERYSTATE_HPP

#include "can/messages/Nissan/Message.hpp"
#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Nissan {

/*
 * Battery state information inspired by a combination of
 *
 * - https://github.com/NiallDarwin/LeafBMU/blob/master/can_data.py
 * - https://docs.google.com/spreadsheets/d/1EHa4R85BttuY4JZ-EnssH4YZddpsDVu6rUFm0P7ouwg/
 * - https://www.mynissanleaf.com/viewtopic.php?f=44&t=11676
 *
 * TODO: Some uncertainty about scalers that we'll need to eliminate
 *
 * Note that this is using aggregated frames as provided by the FrameAggregator.
 */
class BatteryState: public Message
{
public:
   BatteryState(const DataFrame& f);
   BatteryState();

   float getPackVoltage() const;
   float getHealthPercent() const;
   float getCapacityAh() const;
   float getSocPercent() const;

   BatteryState& setHealthPercent(float);
   BatteryState& setSocPercent(float);
   
   virtual void toStream(logging::ostream&) const;

private:
   float m_pack_voltage;
   float m_health_pc;
   float m_capacity_ah;
   float m_soc_pc;
};

}
}
}

#endif // _CAN_MESSAGES_NISSAN_BATTERYSTATE_HPP
