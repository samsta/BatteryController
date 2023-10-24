#include "InverterHeartbeat.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace SINEX {

InverterHeartbeat::InverterHeartbeat(const DataFrame& frame):
		Message(ID_INVERTER_HEARTBEAT),
		m_heartbeat_value()
{
	if (frame.id() != id()) return;
	if (frame.size() != 8) return;

	m_heartbeat_value = frame.getUnsignedShort(0,can::DataFrame::ByteOrder::LSB_FIRST);
	setValid();
}

InverterHeartbeat::InverterHeartbeat():
		Message(ID_INVERTER_HEARTBEAT),
		m_heartbeat_value()
{
}

uint16_t InverterHeartbeat::getHeartbeatValue() const
{
   return m_heartbeat_value;
}

void InverterHeartbeat::toStream(logging::ostream& os) const
{
   os << "InverterHeartbeat: ";
   if (not valid())
   {
      os << "invalid";
   }
   // else
   // {
   //    os << "InfoType=";
   //    switch (getInfoType())
   //    {
   //    case ENSEMBLE:
   //       os << "ENSEMBLE";
   //       break;
   //    case SYSTEM_EQUIPMENT:
   //       os << "SYSTEM_EQUIPMENT";
   //       break;
   //    default:
   //       os << "Unknown";
   //       break;
   //    }
   // }
}


}
}
}
