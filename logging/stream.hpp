/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _LOGGING_STREAM_HPP
#define _LOGGING_STREAM_HPP

#include "config.h"

#ifdef HAS_STD_IOSTREAM
#include <iostream>
#else
#include "logging/no_stl/stream.hpp"
#endif

namespace logging {

#ifdef HAS_STD_IOSTREAM
typedef std::ostream ostream;
#else
typedef no_stl::ostream ostream;
#endif

}

#endif // _LOGGING_STREAM_HPP
