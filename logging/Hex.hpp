/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _LOGGING_HEX_HPP
#define _LOGGING_HEX_HPP

#include "config.h"
#include <stdint.h>

#ifdef HAS_STD_IOSTREAM
#include <ostream>
#endif
#include "logging/no_stl/stream.hpp"

namespace logging {

class Hex
{
public:
   explicit Hex(uint32_t v): v(v){}
   uint32_t v;
};

class HexByte
{
public:
   explicit HexByte(uint8_t v): v(v){}
   uint8_t v;
};

#ifdef HAS_STD_IOSTREAM
std::ostream& operator<<(std::ostream& os, const Hex& h);
std::ostream& operator<<(std::ostream& os, const HexByte& h);
#endif

logging::no_stl::ostream& operator<<(logging::no_stl::ostream& os, const Hex& h);
logging::no_stl::ostream& operator<<(logging::no_stl::ostream& os, const HexByte& h);

}

#endif // _LOGGING_HEX_HPP
