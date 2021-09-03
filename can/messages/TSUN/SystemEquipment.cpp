#include "SystemEquipment.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

SystemEquipment::SystemEquipment(const DataFrame& frame):
		Message(ID_INVERTER_REQUEST_SYS_EQUIP_INFO)
{
	if (frame.id() != ID_INVERTER_REQUEST_INFORMATION) return;
	if (frame.size() != 8) return;
	// the first byte must be 2 for this to be a SystemEquipment msg
	if (frame.getByte(0) != 2) return;

	setValid();
}

}
}
}
