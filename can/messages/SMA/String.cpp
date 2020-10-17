/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "String.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "can/FrameSink.hpp"
#include "util.hpp"
#include <string.h>

using namespace util;

namespace can {
namespace messages {
namespace SMA {
namespace {
const unsigned CHARS_PER_MESSAGE = 7;
}

String::String(Ids id):
   StandardDataFrame(id, "0000000000000000")
{
}

String::String(Ids id, uint8_t index, const char* string):
   StandardDataFrame(id, "0000000000000000")
{
   setByte(0, index);
   strncpy(reinterpret_cast<char*>(data()+1), string, CHARS_PER_MESSAGE);
}

void sendString(FrameSink& sender, Ids id, const char* string)
{
   if (string == nullptr) return;

   const char* end = string + strlen(string);
   int index = 0;
   while (string < end)
   {
      sender.sink(String(id, index, string));
      string += CHARS_PER_MESSAGE;
      index++;
   }
}

}
}
}
