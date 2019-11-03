/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "logging/Hex.hpp"
#include <iomanip>

namespace logging {

std::ostream& operator<<(std::ostream& os, const Hex& h)
{
   std::ios_base::fmtflags flags = os.flags();
   os << std::hex << std::uppercase << h.v;
   os.flags(flags);
   return os;
}

std::ostream& operator<<(std::ostream& os, const HexByte& h)
{
   std::ios_base::fmtflags flags = os.flags();
   os << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << unsigned(h.v);
   os.flags(flags);
   return os;
}

}
