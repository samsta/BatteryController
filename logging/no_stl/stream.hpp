/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _LOGGING_NO_STL_STREAM_HPP_
#define _LOGGING_NO_STL_STREAM_HPP_

#include <stdint.h>

namespace logging {
namespace no_stl {

// no-op functionality to make it compile on the Arduino. Implement later

class ostream
{
};

inline ostream& operator<<(ostream& os, const char*){ return os; }
inline ostream& operator<<(ostream& os, const char&){ return os; }
inline ostream& operator<<(ostream& os, const float&){ return os; }
inline ostream& operator<<(ostream& os, const uint16_t&){ return os; }
inline ostream& operator<<(ostream& os, const uint32_t&){ return os; }

}
}


#endif // _LOGGING_NO_STL_STREAM_HPP_
