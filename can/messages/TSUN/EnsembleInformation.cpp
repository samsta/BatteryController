#include "EnsembleInformation.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

EnsembleInformation::EnsembleInformation(const DataFrame& frame):
		Message(ID_INVERTER_REQUEST_ENSEMBLE)
{
	if (frame.id() != ID_INVERTER_REQUEST_INFORMATION) return;
	if (frame.size() != 8) return;
	if (frame.getByte(0) != ID_INVERTER_REQUEST_ENSEMBLE) return;

	setValid();
}

}
}
}
