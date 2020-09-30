/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "String.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "util.hpp"
#include <string.h>

using namespace util;

namespace can {
namespace messages {
namespace SMA {

String::String(Ids id):
   StandardDataFrame(id, "0000000000000000")
{
}

String::String(Ids id, uint8_t index, const char* string):
   StandardDataFrame(id, "0000000000000000")
{
   setByte(0, index);
   strncpy(reinterpret_cast<char*>(data()+1), string, size()-1);
}



}
}
}
