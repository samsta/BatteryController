/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Contactor.hpp"

namespace contactor {

logging::ostream& operator<<(logging::ostream& os, const Contactor& contactor)
{
   os << "Safe to operate: " << (contactor.isSafeToOperate() ? "Yes" : "No") << std::endl;
   os << "State:           " << (contactor.isClosed() ? "CLOSED" : "OPEN") << std::endl;
   return os;
}

}
