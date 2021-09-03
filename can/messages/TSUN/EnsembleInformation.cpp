#include "EnsembleInformation.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

EnsembleInformation::EnsembleInformation(const DataFrame& frame):
		Message(ID_INVERTER_REQUEST_ENSEMBLE_INFO)
{
	if (frame.id() != ID_INVERTER_REQUEST_INFORMATION) return;
	if (frame.size() != 8) return;
	// the first byte must be 0 for this to be an EnsembleInformation msg
	if (frame.getByte(0) != 0) return;

	setValid();
}

}
}
}
