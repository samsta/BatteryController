#include "InformationRequest.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

InformationRequest::InformationRequest(const DataFrame& frame):
		Message(ID_INVERTER_REQUEST_INFORMATION),
		m_infotype()
{
	if (frame.id() != id()) return;
	if (frame.size() != 8) return;

	if (frame.getByte(0) == ENSEMBLE) {
		m_infotype = ENSEMBLE;
	}
	else if (frame.getByte(0) == SYSTEMEQUIPMENT) {
		m_infotype = SYSTEMEQUIPMENT;
	}
	else return;

	setValid();
}

InformationRequest::InfoType InformationRequest::getInfoType() const
{
   return m_infotype;
}

}
}
}
