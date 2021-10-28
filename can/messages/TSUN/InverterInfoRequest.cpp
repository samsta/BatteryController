#include "InverterInfoRequest.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

InverterInfoRequest::InverterInfoRequest(const DataFrame& frame):
		Message(ID_INVERTER_INFO_REQUEST),
		m_info_type()
{
	if (frame.id() != id()) return;
	if (frame.size() != 8) return;

	switch (frame.getByte(0))
	{
	   case ENSEMBLE:
	   case SYSTEM_EQUIPMENT:
	      m_info_type = InfoType(frame.getByte(0));
	      break;

	   default:
	      return;
	}

	setValid();
}

InverterInfoRequest::InverterInfoRequest():
		Message(ID_INVERTER_INFO_REQUEST),
		m_info_type()
{
}

InverterInfoRequest::InfoType InverterInfoRequest::getInfoType() const
{
   return m_info_type;
}

void InverterInfoRequest::toStream(logging::ostream& os) const
{
   os << "InverterInfoRequest: ";
   if (not valid())
   {
      os << "invalid";
   }
   else
   {
      os << "InfoType=";
      switch (getInfoType())
      {
      case ENSEMBLE:
         os << "ENSEMBLE";
         break;
      case SYSTEM_EQUIPMENT:
         os << "SYSTEM_EQUIPMENT";
         break;
      default:
         os << "Unknown";
         break;
      }
   }
}


}
}
}
