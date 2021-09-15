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

InverterInfoRequest::InfoType InverterInfoRequest::getInfoType() const
{
   return m_info_type;
}

}
}
}
