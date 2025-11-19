/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "Contactor.hpp"

namespace contactor {

logging::ostream& operator<<(logging::ostream& os, const Contactor& contactor)
{
   os << "Inverter Comms Ok:    " << (contactor.inverterCommsOk() ? "Yes" : "No") << std::endl;
   os << "Cont Safe To Operate: " << (contactor.isSafeToOperate() ? "Yes" : "No") << std::endl;
   os << "Contactor State:      " << (contactor.isClosed() ? "CLOSED" : "OPEN") << std::endl;
   return os;
}

}
