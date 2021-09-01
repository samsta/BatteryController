#include "EnsembleInformation.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

EnsembleInformation::EnsembleInformation(const DataFrame&):
		Message(ID_ENSEMBLE_INFORMATION)
{}

bool EnsembleInformation::valid() const
{
   return true;
}

}
}
}
