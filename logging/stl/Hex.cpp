/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "logging/Hex.hpp"
#include <iomanip>

namespace logging {

std::ostream& operator<<(std::ostream& os, const Hex& h)
{
   return os << std::hex << std::uppercase << h.v;
}

std::ostream& operator<<(std::ostream& os, const HexByte& h)
{
   return os << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << unsigned(h.v);
}

}
